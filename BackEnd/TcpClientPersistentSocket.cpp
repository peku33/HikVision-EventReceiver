#include "TcpClientPersistentSocket.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "ErrnoException.hpp"

TcpClientPersistentSocket::TcpClientPersistentSocket(EPoll & EP, const Ip4Port & ConnectTo, const OnConnected_f & OnConnected, const OnNewData_f & OnNewData, const OnError_f & OnError, const KeepAliveDataOpt_t & KeepAliveDataOpt):
	ConnectTo(ConnectTo), OnConnected(OnConnected), OnNewData(OnNewData), OnError(OnError),
	Connected(false),
	EPF(EP, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLERR, std::bind(&TcpClientPersistentSocket::OnEPoll, this, std::placeholders::_1))
{
	EPF.Acquire(socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | O_NONBLOCK, IPPROTO_TCP));
	
	if(KeepAliveDataOpt)
	{
		const KeepAliveData_t & KeepAliveData = *KeepAliveDataOpt;

		const int TrueInt = 1;
		if(setsockopt(EPF, SOL_SOCKET, SO_KEEPALIVE, &TrueInt, sizeof(TrueInt)) != 0)
			throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() SO_KEEPALIVE");
		
		if(setsockopt(EPF, SOL_TCP, TCP_KEEPCNT, &KeepAliveData.Probes, sizeof(KeepAliveData.Probes)) != 0)
			throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() TCP_KEEPCNT");
		if(setsockopt(EPF, SOL_TCP, TCP_KEEPIDLE, &KeepAliveData.FirstTime, sizeof(KeepAliveData.FirstTime)) != 0)
			throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() TCP_KEEPIDLE");
		if(setsockopt(EPF, SOL_TCP, TCP_KEEPINTVL, &KeepAliveData.Interval, sizeof(KeepAliveData.Interval)) != 0)
			throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() TCP_KEEPINTVL");
		
	}
	
	const sockaddr_in & ConnectAddr = ConnectTo.GetSockAddrIn();
	const int ConnectResult = connect(EPF, (struct sockaddr *) &ConnectAddr, sizeof(ConnectAddr));
	if(ConnectResult != 0 && errno != EINPROGRESS)
		throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() connect()");
	
	EPF.Enable();
}

void TcpClientPersistentSocket::OnEPoll(const uint32_t Events)
{
	if(Events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
	{
		OnError();
	}
	else
	{
		if(Events & EPOLLOUT)
		{
			if(!Connected)
			{
				Connected = true;
				OnConnected();
			}
			
			EPF.ChangeEvents(EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR);
		}
		
		if(Events & EPOLLIN)
		{
			char InputBuffer[InputBufferLength];
			const ssize_t DataLength = recv(EPF, InputBuffer, InputBufferLength, 0);
			if(DataLength <= 0)
				throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() OnEPoll()");
			
			OnNewData(std::string(InputBuffer, DataLength));
		}
	}
}

void TcpClientPersistentSocket::Send(const char * const Data, const size_t Length)
{
	if(send(EPF, Data, Length, 0) != (ssize_t) Length)
		throw ErrnoException(errno, "TcpClientPersistentSocket::TcpClientPersistentSocket() Send()");
}
void TcpClientPersistentSocket::Send(const std::string & Data)
{
	Send(Data.c_str(), Data.length());
}