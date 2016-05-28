#pragma once

#include <string>
#include <boost/optional.hpp>

#include "Ip4.hpp"

/**
 * @brief Wrapper class for Ip4 + Port
 * @details + some basic operations
 */
class Ip4Port
{
	//Probably not the most effective way, but allows functional-only access to Ip and Port in constructor initializer list
	private:
		static std::string ExtractIpFromIpPortStr(const std::string & IpPortStr);
		static uint16_t ExtractPortFromIpPortStr(const std::string & IpPortStr);

	public:
		/**
		 * @brief Constructor, constructs object from Ip and Port
		 * 
		 * @param Ip Ip
		 * @param Port Port, host (local, standard, ...) byte order
		 */
		Ip4Port(const Ip4 & Ip, const uint16_t Port);
		/**
		 * @brief Constructor, constructs from string representation of ip and port
		 * 
		 * @param IpPortStr IpPortStr is expected to be dotted-colon notation, (x.x.x.x:xxxx)
		 */
		Ip4Port(const std::string & IpPortStr);
		/**
		 * @brief Constructor, constructs from sockaddr_in structure
		 * 
		 * @param SockAddrIn Input structure
		 */
		Ip4Port(const sockaddr_in & SockAddrIn);

		//Nothing to say
		bool operator == (const Ip4Port & Other) const;
		bool operator != (const Ip4Port & Other) const;

	private:
		Ip4 Ip;
		uint16_t Port; //Host order

	private:
		mutable std::string IpPortStr;
		mutable boost::optional<sockaddr_in> SockAddrIn;

	public:
		/**
		 * @brief Returns Ip4
		 * @return Ip4
		 */
		const Ip4 & GetIp() const;
		/**
		 * @brief Returns port
		 * @return Port, host byte order
		 */
		uint16_t GetPort() const;

		/**
		 * @brief Returns string representation of Ip4Port
		 * @details Uses lazy-initialized cache
		 * @return String representation in dotted-colon form (see constructor)
		 */
		const std::string & GetIpPortStr() const;
		/**
		 * @brief Returns sockaddr_in for this object
		 * @details Uses lazy-initialized cache
		 * @return sockaddr_in for this object
		 */
		const sockaddr_in & GetSockAddrIn() const;
};