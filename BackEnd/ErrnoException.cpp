#include "ErrnoException.hpp"

ErrnoException::ErrnoException(int _errno, const std::string & Where): 
	std::runtime_error(std::string() + "ErrnoException: " + ErrnoToString(_errno) + " (" + std::to_string(_errno) + ") (" + Where + ")")
{

}

std::string ErrnoException::ErrnoToString(int _errno)
{
	char Buffer[128];
	char * Result = strerror_r(_errno, Buffer, sizeof(Buffer));

	//According to documentation 'returns a pointer to a string containing the error message. This may be either a pointer to a string that the function stores in buf, or a pointer to some (immutable) static string (in which case buf is unused).'
	return std::string(Result);
}