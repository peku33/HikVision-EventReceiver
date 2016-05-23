#include "Fd.hpp"

#include <stdexcept>
#include <unistd.h>

#include "ErrnoException.hpp"

Fd::Fd(): TheFd(-1)
{
	
}
Fd::~Fd()
{
	if(IsAcquired())
		Close();
}

bool Fd::IsAcquired() const
{
	return TheFd >= 0;
}
void Fd::Acquire(int NewFd)
{
	if(NewFd < 0)
		throw std::invalid_argument("NewFd < 0");

	if(IsAcquired())
		throw std::logic_error("Fd::Acquire() while IsAcquired()");

	TheFd = NewFd;
}
void Fd::Close()
{
	if(!IsAcquired())
		throw std::logic_error("Fd::Acquire() while IsAcquired()");

	if(close(TheFd) != 0)
		throw ErrnoException(errno);

	TheFd = -1;
}

int Fd::Release()
{
	if(!IsAcquired())
		throw std::logic_error("Fd::Release() while !IsAcquired()");

	int OldFd = TheFd;
	TheFd = -1;
	return OldFd;
}
Fd::operator int () const
{
	if(!IsAcquired())
		throw std::logic_error("Fd::operator int () while !IsAcquired()");

	return TheFd;
}