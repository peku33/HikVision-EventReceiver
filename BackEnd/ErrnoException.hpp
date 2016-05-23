#pragma once

#include <string>
#include <stdexcept>
#include <string.h>

/**
 * @brief An exception used to represent system call errors that populate errno variable. Its value is translated into human-readable message
 */
class ErrnoException : public std::runtime_error
{
	public:
		ErrnoException(int _errno, const std::string & Where);

	private:
		static std::string ErrnoToString(int _errno);
};