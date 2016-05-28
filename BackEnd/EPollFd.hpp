#pragma once

#include <functional>

#include "Fd.hpp"

class EPoll;

class EPollFd : public Fd
{
	public:
		typedef std::function<void (const uint32_t)> OnEPoll_f;

	public:
		EPollFd(EPoll & EP, const uint32_t Events, const OnEPoll_f & OnEPoll);
		~EPollFd();

		//Copy, move constructors implicitly deleted because of Fd base.

	private:
		EPoll & EP;

	private:
		friend EPoll;
		
		uint32_t Events;
		const OnEPoll_f OnEPoll;

	private:
		bool Enabled;

	public:
		void Acquire(const int NewFd);
		void Close();

	public:
		void Enable();
		void Disable();
		void ChangeEvents(const uint32_t NewEvents);
};