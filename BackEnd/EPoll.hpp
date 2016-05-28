#pragma once

#include <signal.h>
#include <unordered_set>

#include "NoCopy.hpp"
#include "Fd.hpp"

class EPollFd;

class EPoll
{
	public:
		EPoll();

	private:
		/**
		 * @brief To catch SIGINT / SIGTERM / SIGQUIT, ex. ctrl+c, etc
		 */
		static thread_local volatile sig_atomic_t ExitFlag;
		static void SigHandler(int);

	private:
		friend class EPollFd;
		void EPollFdRegister(EPollFd * EPF);
		void EPollFdModify(EPollFd * EPF);
		void EPollFdUnRegister(EPollFd * EPF);

	private:
		static const unsigned int EPollEventQueueLength = 64;

		//Fd class maintaining epoll structure
		Fd EP;

	private:
		std::unordered_set<EPollFd *> RemovedEPFs;

	public:
		void Main();
};