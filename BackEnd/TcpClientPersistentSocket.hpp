#pragma once

#include <boost/optional.hpp>

#include "Ip4Port.hpp"
#include "EPollFd.hpp"

class EPoll;

/**
 * @brief Class representing event-driven tcp socket. This is a persistent socket indended to work with HikVision AlertStream.
 * @details During construction connection is scheduled to be established. See constructor for more details.
 */
class TcpClientPersistentSocket
{
	public:
		/**
		 * @brief Called once when connection is established.
		 */
		typedef std::function<void (void)> OnConnected_f;
		/**
		 * @brief Called when new data arrives on socket from remote endpoint. The parameter is raw data packed into std::string
		 */
		typedef std::function<void (const std::string &)> OnNewData_f;
		/**
		 * @brief Called when error (of any type) occurs on socket. After this call this object must be immediately destroyed (and possibly re-created).
		 */
		typedef std::function<void (void)> OnError_f;

		/**
		 * @brief KeepAlive parameters
		 */
		struct KeepAliveData_t
		{
			int Probes; //TCP_KEEPCNT - The maximum number of keepalive probes TCP should send before dropping the connection.
			int FirstTime; //TCP_KEEPIDLE - The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes, if the socket option SO_KEEPALIVE has been set on this socket.
			int Interval; //TCP_KEEPINTVL - The time (in seconds) between individual keepalive probes.
		};

		/**
		 * @brief KeepAlive parameters or nothing. If nothing - KeepAlive would not be enabled
		 */
		typedef boost::optional<KeepAliveData_t> KeepAliveDataOpt_t;

	private:
		static const unsigned int InputBufferLength = 1500;

	public:
		/**
		 * @brief Constructor, starts connection
		 * @details During construction the connection is started, but it does not mean that after constructor the connection is ready. Constructor will return immediately, allowing full non-blocking operation. The connection is ready, when OnConnected() gots called. After construction the first to be callback would be OnConnected() or OnError() or *nothing* (no callback never) when there is no KeepAlive and connection fails. User of this class should have his own timeout system (based on SingleTimer for example)
		 * 
		 * @param EP EPoll to register this socket in
		 * @param ConnectTo Ip and Port to establish connection to
		 * @param OnConnected Callback to be called when connection is established (and ready for Send() for example), seeOnConnected_f
		 * @param OnNewData Callback to be called when new data arrives, see OnNewData_f
		 * @param OnError Callback to be called when error of any type occurs. After OnError() user is required to destroy connection immediately.
		 * @param KeepAliveDataOpt Optional KeepAlive parameters. See KeepAliveData_t and KeepAliveDataOpt_t
		 */
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
		/**
		 * @brief Sends data to socket. Socket must be connected (OnConnected() was called) before sending
		 * 
		 * @param Data Raw bytes to send
		 * @param Length Length of data
		 */
		void Send(const char * const Data, const size_t Length);

		/**
		 * @brief See Send(const char * const Data, const size_t Length);
		 * 
		 * @param Data will be used as input for Send(const char * const Data, const size_t Length);
		 */
		void Send(const std::string & Data);
};