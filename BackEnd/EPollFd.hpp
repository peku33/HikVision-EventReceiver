#pragma once

#include <functional>

#include "Fd.hpp"
#include "EPoll.hpp"

class EPollFd : public Fd
{
	public:
		typedef std::function<void (const uint32_t)> OnEPoll_f;

	private:
		friend EPoll;
		
		uint32_t Events;
		const OnEPoll_f OnEPoll;
};