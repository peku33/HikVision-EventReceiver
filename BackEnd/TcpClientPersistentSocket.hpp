#pragma once

#include <boost/optional.hpp>
//#include <chrono>

#include "Ip4Port.hpp"
#include "EPollFd.hpp"

class EPoll;

class TcpClientPersistentSocket
{
	public:
		typedef std::function<void (void)> OnConnected_f;
		typedef std::function<void (const std::string &)> OnNewData_f;
		typedef std::function<void (void)> OnError_f;

		struct KeepAliveData_t
		{
			int Probes; //TCP_KEEPCNT - The maximum number of keepalive probes TCP should send before dropping the connection.
			int FirstTime; //TCP_KEEPIDLE - The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes, if the socket option SO_KEEPALIVE has been set on this socket.
			int Interval; //TCP_KEEPINTVL - The time (in seconds) between individual keepalive probes.
		};

		typedef boost::optional<KeepAliveData_t> KeepAliveDataOpt_t;

	private:
		static const unsigned int InputBufferLength = 1500;

	public:
		TcpClientPersistentSocket(EPoll & EP, const Ip4Port & ConnectTo, const OnConnected_f & OnConnected, const OnNewData_f & OnNewData, const OnError_f & OnError, const KeepAliveDataOpt_t & KeepAliveDataOpt = KeepAliveDataOpt_t());

	private:
		const Ip4Port ConnectTo;

		const OnConnected_f OnConnected;
		const OnNewData_f OnNewData;
		const OnError_f OnError;

	private:
		bool Connected;

	private:
		EPollFd EPF;
		void OnEPoll(const uint32_t Events);

	public:
		void Send(const char * const Data, const size_t Length);
		void Send(const std::string & Data);
};