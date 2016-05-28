#pragma once

#include <string>
#include <netinet/in.h>

class Ip4
{
	private:
		static std::string InAddrToIpStr(const in_addr & InAddr);
		static in_addr IpStrToInAddr(const std::string & IpStr);

	public:
		Ip4(const std::string & IpStr);
		Ip4(const in_addr & InAddr);

		bool operator == (const Ip4 & Other) const;
		bool operator != (const Ip4 & Other) const;

	private:
		in_addr InAddr;

	private:
		mutable std::string IpStr;

	public:
		const std::string & GetIpStr() const;
		const in_addr & GetInAddr() const;
};