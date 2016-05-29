#include "TimerSingle.hpp"

#include <stdexcept>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "ErrnoException.hpp"

TimerSingle::TimerSingle(EPoll & EP, const std::chrono::milliseconds & Duration, const OnTimer_f & OnTimer):
	EPF(EP, EPOLLIN, std::bind(&TimerSingle::OnEPoll, this, std::placeholders::_1)), TimerSpec(DurationToItimerspec(Duration)), OnTimer(OnTimer)
{
	const int TimerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if(TimerFd == -1)
		throw ErrnoException(errno, "TimerSingle::TimerSingle()");

	EPF.Acquire(TimerFd);
	EPF.Enable();
}
void TimerSingle::OnEPoll(const uint32_t Events)
{
	if(Events & EPOLLIN)
	{
		uint64_t Count = 0;

		//The timer could have been reset in the meantime.
		//If EAGAIN / EWOULDBLOCK occurs - we have Count = 0 and won't execute event handler
		if((read(EPF, &Count, sizeof(Count)) != sizeof(Count)) && errno != EAGAIN && errno != EWOULDBLOCK)
			throw ErrnoException(errno, "TimerSingle::OnEPoll()");
		
		//Not sure how it could be possible, but we check that
		if(Count > 1)
			throw std::runtime_error("Count > 1 ( = " + std::to_string(Count) + ") (TimerSingle::OnEPoll())");
		
		//If 0 - error occured, we don't call handler
		if(Count > 0)
			OnTimer();
	}
}

void TimerSingle::Restart()
{
	if(timerfd_settime(EPF, 0, &TimerSpec, NULL) != 0)
		throw ErrnoException(errno, "TimerSingle::Restart()");

}
void TimerSingle::Stop()
{
	const struct itimerspec ITS = {{0, 0}, {0, 0}};
	if(timerfd_settime(EPF, 0, &ITS, NULL) != 0)
		throw ErrnoException(errno, "TimerSingle::Stop()");
}

itimerspec TimerSingle::DurationToItimerspec(const std::chrono::milliseconds & Duration)
{
	const std::chrono::seconds FullSeconds = std::chrono::duration_cast<std::chrono::seconds>(Duration);
	const std::chrono::nanoseconds RemainingNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(Duration) - std::chrono::duration_cast<std::chrono::nanoseconds>(FullSeconds);
		
	return {{0, 0}, {(long int) FullSeconds.count(), (long int) RemainingNanoseconds.count()}};
}