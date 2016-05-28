#include "EPollFd.hpp"

#include <stdexcept>

#include "EPoll.hpp"

EPollFd::EPollFd(EPoll & EP, const uint32_t Events, const OnEPoll_f & OnEPoll):
	EP(EP), Events(Events), OnEPoll(OnEPoll), Enabled(false)
{

}
EPollFd::~EPollFd()
{
	if(Enabled)
		Disable();
}

void EPollFd::Acquire(const int NewFd)
{
	return Fd::Acquire(NewFd);
}
void EPollFd::Close()
{
	if(Enabled)
		Disable();

	return Fd::Close();
}

void EPollFd::Enable()
{
	if(Enabled)
		throw std::logic_error("Enabled (EPollFd::Enable())");

	if(!IsAcquired())
		throw std::logic_error("!IsAcquired() (EPollFd::Enable())");

	EP.EPollFdRegister(this);
	Enabled = true;
}
void EPollFd::Disable()
{
	if(!Enabled)
		throw std::logic_error("!Enabled (EPollFd::Disable())");

	if(!IsAcquired())
		throw std::logic_error("!IsAcquired() (EPollFd::Disable())");

	EP.EPollFdUnRegister(this);
	Enabled = false;
}
void EPollFd::ChangeEvents(const uint32_t NewEvents)
{
	Events = NewEvents;

	if(Enabled)
		EP.EPollFdModify(this);
}