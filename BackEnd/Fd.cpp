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
		throw std::invalid_argument("NewFd < 0 (Fd::Acquire())");

	if(IsAcquired())
		throw std::logic_error("IsAcquired() (Fd::Acquire())");

	TheFd = NewFd;
}
void Fd::Close()
{
	if(!IsAcquired())
		throw std::logic_error("IsAcquired() (Fd::Close())");

	if(close(TheFd) != 0)
		throw ErrnoException(errno, "Fd::Close()");

	TheFd = -1;
}

int Fd::Release()
{
	if(!IsAcquired())
		throw std::logic_error("!IsAcquired() (Fd::Release())");

	int OldFd = TheFd;
	TheFd = -1;
	return OldFd;
}
Fd::operator int () const
{
	if(!IsAcquired())
		throw std::logic_error("!IsAcquired() (Fd::operator int ())");

	return TheFd;
}