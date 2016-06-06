#pragma once

#include <signal.h>
#include "NoCopy.hpp"
#include "Fd.hpp"

#include <unordered_set>

class EPollFd;

/**
 * @brief Main event-loop class managing whole application.
 * @details This class is used in two cases:
 * 				- at beginning of application code, user creates instance of this class. When initialization of application components is complete - user should call Main() function. This would pass the control to the main event-loop. Main() would run until exit condition (ex. CTRL+C) occurs. All events are handled inside Main() function. Inside Main() epoll mechanisms waits until at least one event (see EPollFd class) is ready to be processed. Then it passes the control to its callback method. Then it continues the loop, and so on
 * 				- by EPollFd class. Each EPollFd object 'registers' itself in EPoll. When EPollFd object is created - it adds itself to list of watched fds in EPoll. On destruction, fd is removed.
 */
class EPoll
{
	public:
		/**
		 * @brief Constructor. Perfors some basic initialization.
		 * @details Registers signal handlers, spawns epoll structure
		 */
		EPoll();

	private:
		/**
		 * Flag indicating whether event loop should exit.
		 * At application startup it is set to false.
		 * Is beeing set by signal / exit condition
		 */
		static thread_local volatile sig_atomic_t ExitFlag;

		/**
		 * @brief Callback function for exit signals
		 * @details Sets ExitFlag to true
		 */
		static void SigHandler(int);

	/**
	 * Methods to be used by EPollFd
	 */
	friend EPollFd;
	private:
		void EPollFdRegister(EPollFd * EPF);
		void EPollFdModify(EPollFd * EPF);
		void EPollFdUnRegister(EPollFd * EPF);

		/**
		 * In case Fd was closed during EPoll loop, we keep the list of their pointers.
		 * Those Fds would be skipped.
		 */
		std::unordered_set<EPollFd *> RemovedEPFs;
	/**
	 * epoll objects
	 */
	private:
		static const unsigned int EPollEventQueueLength = 64;

		//Fd class maintaining epoll structure
		Fd EP;

	public:
		/**
		 * @brief The (almost) infinite event loop.
		 * @details User should call it when first set of EPollFd is set up (as they are going to do the reset). Returns when exit condition occurs
		 */
		void Main();
};