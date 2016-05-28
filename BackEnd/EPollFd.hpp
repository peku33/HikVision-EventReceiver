#pragma once

#include <functional>

#include "Fd.hpp"

class EPoll;

/**
 * @brief EPollFd is extension for Fd class providing event-watching mechanism
 * @details Upon construction, acquiring fd (like for Fd class) and enabling (by Enable()) the EPoll class listens for events on managed fd. When epoll event (mask described by Events constructor parameter) occurs, OnEPoll() functor is being called.
 */
class EPollFd : public Fd
{
	public:
		/**
		 * @brief Delegate function object provided by class using EPollFd.
		 */
		typedef std::function<void (const uint32_t)> OnEPoll_f;

	public:
		/**
		 * @brief Constructor
		 * @details In fact it does nothing, only stores parameters in local variables for later use. See Acquire(), Enable() for what-you-expect-constructor-will-do
		 * 
		 * @param EP Reference to EPoll event loop (in Enable() we call register itself in it)
		 * @param Events List (bit sum) of EPOLL* (EPOLLIN, EPOLLOUT, etc) events to be passed to epoll
		 * @param OnEPoll Functor (callback) to be called when any of Events occurs
		 */
		EPollFd(EPoll & EP, const uint32_t Events, const OnEPoll_f & OnEPoll);

		/**
		 * @brief Ddestructor
		 * @details If EPollFd is Enabled (registered in EPoll) it is going to be Disabled. After destruction no furher calls to OnEPoll() would be made
		 */
		~EPollFd();

		//Copy, move constructors implicitly deleted because of Fd base.

	//We make EPoll our friend, to allow access to Events, OnEPoll, etc.
	friend EPoll;

	private:
		EPoll & EP;
		uint32_t Events;
		const OnEPoll_f OnEPoll;

		//Is this EPollFd currently registered in EPoll? false after construction, true after Enable(), ...
		bool Enabled;

	public:
		//See base class (Fd) for IsAcquired(), Acquire(), Release();

		/**
		 * @brief Overrides Fd::Close(). Disables() this if Enabled, then calls Fd::Close()
		 */
		void Close();
		/**
		 * @brief We don't allow to release EPollFd
		 */
		void Release() = delete;

	public:
		/**
		 * @brief Enables this EPollFd
		 * @details Enabling means registering (adding) to EPoll event loop. Before calling Enable() EPoll is not aware of this object existence.
		 */
		void Enable();
		/**
		 * @brief Disables this EPollFd
		 * @details Deregisters this object from EPoll. After calling Disable() this object won't be accessed by EPoll. 
		 */
		void Disable();
		/**
		 * @brief Changes Events mask
		 * @details Changes epoll events mask. This overwrites Events passed in constructor
		 * 
		 * @param NewEvents New mask
		 */
		void ChangeEvents(const uint32_t NewEvents);
};