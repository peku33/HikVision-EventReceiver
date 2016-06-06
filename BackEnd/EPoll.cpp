#include "EPoll.hpp"

#include <stdexcept>
#include <sys/epoll.h>
#include <string.h>

#include "EPollFd.hpp"
#include "ErrnoException.hpp"

#ifndef DEBUG
	#include <iostream>
#endif

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

void EPoll::EPollFdRegister(EPollFd * EPFPtr)
{
	epoll_data_t EPollData;
	EPollData.ptr = EPFPtr;
	
	epoll_event EPollEvent;
	EPollEvent.events = (*EPFPtr).Events;
	EPollEvent.data = EPollData;
	
	if(epoll_ctl(EP, EPOLL_CTL_ADD, (*EPFPtr), &EPollEvent) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdRegister()");
}
void EPoll::EPollFdModify(EPollFd * EPFPtr)
{
	epoll_data_t EPollData;
	EPollData.ptr = EPFPtr;
	
	epoll_event EPollEvent;
	EPollEvent.events = (*EPFPtr).Events;
	EPollEvent.data = EPollData;
	
	if(epoll_ctl(EP, EPOLL_CTL_MOD, (*EPFPtr), &EPollEvent) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdModify()");
}
void EPoll::EPollFdUnRegister(EPollFd * EPFPtr)
{
	if(epoll_ctl(EP, EPOLL_CTL_DEL, (*EPFPtr), NULL) != 0)
		throw ErrnoException(errno, "EPoll::EPollFdUnRegister()");

	if(!RemovedEPFs.insert(EPFPtr).second)
		throw ErrnoException(errno, "Duplicated EPFPtr in RemovedEPFs EPoll::EPollFdUnRegister()");		
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

				//See RemovedEPFs description
				if(!RemovedEPFs.empty() && RemovedEPFs.count(EPFPtr))
					continue;

				const uint32_t Events = EPollEvents[I].events;

				//If this FD has already been removed, skip
				if(!(*EPFPtr).IsAcquired())
					continue;

				if(Events == 0)
					throw std::logic_error("EPoll returned 0 events (EPoll::Main())");

				if((Events & ~EPFPtr->Events))
					throw std::runtime_error("EPoll returned event not handled by EPollFd (EPoll::Main())");

				#ifdef DEBUG

					//For debug version we want the application to be immediately killed by unhandled exception.
					//This is the easiest way to find out there is something wrong (and what)
					(*EPFPtr).OnEPoll(Events);

				#else

					//For production we handle each exception we can, to keep the application going
					//Ususally stack unwinding would clean the mess created before exception was thrown

					try
					{
						(*EPFPtr).OnEPoll(Events);
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