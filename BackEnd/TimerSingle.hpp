#pragma once

#include <chrono>
#include <functional>
#include <sys/time.h>

#include "EPollFd.hpp"

class EPoll;

class TimerSingle
{
	public:
		typedef std::function<void (void)> OnTimer_f;

	public:
		TimerSingle(EPoll & EP, const std::chrono::milliseconds & Duration, const OnTimer_f & OnTimer);

	private:
		EPollFd EPF;
		void OnEPoll(const uint32_t Events);

	private:
		const itimerspec TimerSpec;
		const OnTimer_f OnTimer;

	public:
		void Restart();
		void Stop();

	private:
		static itimerspec DurationToItimerspec(const std::chrono::milliseconds & Duration);
};