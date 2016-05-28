#include "EPoll.hpp"

#include <stdexcept>
#include <sys/epoll.h>
#include <string.h>

#include "EPollFd.hpp"
#include "ErrnoException.hpp"

#include <iostream>

thread_local volatile sig_atomic_t EPoll::ExitFlag = 0;
void EPoll::SigHandler(int)
{
	ExitFlag = 1;
}

EPoll::EPoll()
{
	EP.Acquire(epoll_create(EPOLL_CLOEXEC));

	//We block all signals except those below. Listed signals are going to be handled by SigHandler all others are blocked.
	auto Signals = {SIGINT, SIGQUIT, SIGTERM};

	sigset_t MaskedSignals;
	sigfillset(&MaskedSignals);

	struct sigaction Action;
	memset(&Action, 0, sizeof(Action));
	Action.sa_handler = SigHandler;

	//For each 'handled' signal, we delete it from blocked set and install a handler
	for(auto Signal : Signals)
	{
		sigdelset(&MaskedSignals, Signal);
		sigaction(Signal, &Action, NULL);
	}

	pthread_sigmask(SIG_SETMASK, &MaskedSignals, NULL);
}

void EPoll::EPollFdRegister(EPollFd * EPF)
{
	epoll_data_t EPollData;
	EPollData.ptr = EPF;
	
	epoll_event EPollEvent;
	EPollEvent.events = EPF->Events;
	EPollEvent.data = EPollData;
	
	if(epoll_ctl(EP, EPOLL_CTL_ADD, EPF->TheFd, &EPollEvent) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdRegister()");
}
void EPoll::EPollFdModify(EPollFd * EPF)
{
	epoll_data_t EPollData;
	EPollData.ptr = EPF;
	
	epoll_event EPollEvent;
	EPollEvent.events = EPF->Events;
	EPollEvent.data = EPollData;
	
	if(epoll_ctl(EP, EPOLL_CTL_MOD, EPF->TheFd, &EPollEvent) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdModify()");
}
void EPoll::EPollFdUnRegister(EPollFd * EPF)
{
	if(epoll_ctl(EP, EPOLL_CTL_DEL, EPF->TheFd, NULL) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdUnRegister()");
	
	if(!RemovedEPFs.insert(EPF).second)
		throw std::logic_error("!RemovedUEPFs.insert(EPF).second (EPoll::EPollFdUnRegister())");
}

void EPoll::Main()
{
	while(!ExitFlag)
	{
		epoll_event EPollEvents[EPollEventQueueLength];
		const int EPollResult = epoll_wait(EP, EPollEvents, EPollEventQueueLength, -1);

		if(EPollResult < 0)
		{
			if(errno == EINTR)
			{
				ExitFlag = true;
				continue;
			}
		}
		else if(EPollResult > 0)
		{
			RemovedEPFs.clear();

			for(int I = 0; I < EPollResult; I++)
			{
				EPollFd * EPFPtr = reinterpret_cast<EPollFd *>(EPollEvents[I].data.ptr);
				const uint32_t Events = EPollEvents[I].events;

				//If this FD has already been removed, skip
				if(RemovedEPFs.count(EPFPtr))
					continue;

				if(Events == 0)
					throw std::logic_error("EPoll returned 0 events (EPoll::Main())");

				if((Events & ~EPFPtr->Events))
					throw std::runtime_error("EPoll returned event not handled by EPollFd (EPoll::Main())");

				#ifdef DEBUG

					//For debug version we want the application to be immediately killed by unhandled exception.
					//This is the easiest way to find out there is something wrong (and what)
					EPFPtr->OnEPoll(Events);

				#else

					//For production we handle each exception we can, to keep the application going
					//Ususally stack unwinding would clean the mess created before exception was thrown

					try
					{
						EPFPtr->OnEPoll(Events);
					}
					catch(const std::exception & e)
					{
						std::cerr << "Exception for fd: " << *EPFPtr << " - " << e.what() << std::endl;
					}

				#endif
			}
		}
	}
}