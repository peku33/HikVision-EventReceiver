#pragma once

#include <chrono>
#include <functional>
#include <sys/time.h>

#include "EPollFd.hpp"

class EPoll;

/**
 * @brief Class representing Single Timer. 'Single' means, that after (Re)Starting it will call OnTimer procedure after Duration.
 * @details Timer is not automatically started after construction, so the Restart() must be called.
 */
class TimerSingle
{
	public:
		/**
		 * @brief Callback prototype
		 */
		typedef std::function<void (void)> OnTimer_f;

	public:
		/**
		 * @brief Constructor, creates stopped timer
		 * 
		 * @param EP EPoll class to register the timer in
		 * @param Duration After calling Restart() OnTimer() would be called after Duration
		 * @param OnTimer Callback function to be called when timer hits
		 */
		TimerSingle(EPoll & EP, const std::chrono::milliseconds & Duration, const OnTimer_f & OnTimer);

	private:
		EPollFd EPF;
		void OnEPoll(const uint32_t Events);

	private:
		const itimerspec TimerSpec;
		const OnTimer_f OnTimer;

	public:
		/**
		 * @brief Starts the timer. After Duration (from constructor), OnTimer() would be called
		 * @details If the timer is already running, it will overwrite its current position. If the timer has hit, but in the meantime was restarted, OnTimer won't be called. 
		 */
		void Restart();

		/**
		 * @brief Stops (zeros) current timer.
		 * @details If the timer has not been running, no effect. If the timer has hit, but in the meantime was stopped, OnTimer won't be called.  
		 */
		void Stop();

	private:
		static itimerspec DurationToItimerspec(const std::chrono::milliseconds & Duration);
};