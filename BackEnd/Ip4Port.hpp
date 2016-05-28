#pragma once

#include <string>
#include <boost/optional.hpp>

#include "Ip4.hpp"

class Ip4Port
{
	private:
		static std::string ExtractIpFromIpPortStr(const std::string & IpPortStr);
		static uint16_t ExtractPortFromIpPortStr(const std::string & IpPortStr);

	public:
		Ip4Port(const Ip4 & Ip, const uint16_t Port);
		Ip4Port(const std::string & IpPortStr);
		Ip4Port(const sockaddr_in & SockAddrIn);

		bool operator == (const Ip4Port & Other) const;
		bool operator != (const Ip4Port & Other) const;

	private:
		Ip4 Ip;
		uint16_t Port; //Host order

	private:
		mutable std::string IpPortStr;
		mutable boost::optional<sockaddr_in> SockAddrIn;

	public:
		const Ip4 & GetIp() const;
		uint16_t GetPort() const;

		const std::string & GetIpPortStr() const;
		const sockaddr_in & GetSockAddrIn() const;
};