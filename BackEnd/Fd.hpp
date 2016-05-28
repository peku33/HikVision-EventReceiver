#pragma once

#include "NoCopy.hpp"

/**
 * @brief Smart-pointer-like class for managing linux file descriptor resource. Only one Fd can be held at once.
 */
class Fd : public NoCopy
{
	public:
		/**
		 * @brief Constructor. Initializes empty Fd object to be populated by Acquire() method
		 */
		Fd();

		/**
		 * @brief Destructor. If this object currently holds acquired Fd it will be closed
		 */
		~Fd();

	protected:
		int TheFd;

	public:
		/**
		 * @brief Returns if this object currently holds acquired Fd
		 */
		bool IsAcquired() const;
		/**
		 * @brief Takes ownership (acquires) file descriptor provided by parameter
		 * @details This file descriptor would be maintained by this object and automaticly closed at its life end
		 * 
		 * @param NewFd Fd returned by socket(), pipe(), etc...
		 */
		void Acquire(const int NewFd);
		/**
		 * @brief Closes currently held descriptor. This is called by the destructor if object is not empty.
		 * @details User can call this function to close Fd earilier then at objects life end
		 */
		void Close();

		/**
		 * @brief Releases the Fd to the user without closing it.
		 * @details After calling this method user is responsible to maintaint and close it
		 * @return Fd that was held by this object
		 */
		int Release();
		/**
		 * @brief 'Converts' this object to integer. This allows to pass this object directly to read(), write() etc...
		 * @return Fd held by this object
		 */
		operator int () const;
};