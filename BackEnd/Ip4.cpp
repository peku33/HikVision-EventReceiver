#include "Ip4.hpp"

#include <arpa/inet.h>
#include <stdexcept>

std::string Ip4::InAddrToIpStr(const in_addr & InAddr)
{
	char Output[INET_ADDRSTRLEN];

	if(inet_ntop(AF_INET, &InAddr, Output, INET_ADDRSTRLEN) != Output)
		throw std::invalid_argument("inet_ntop failed (Ip4::InAddrToIpStr())");

	return std::string(Output);
}
in_addr Ip4::IpStrToInAddr(const std::string & IpStr)
{
	in_addr Output;

	if(inet_pton(AF_INET, IpStr.c_str(), &Output) != 1)
		throw std::invalid_argument(std::string() + "inet_pton failed for: " + IpStr + " (Ip4::IpStrToInAddr())");

	return Output;
}

bool Ip4::operator == (const Ip4 & Other) const
{
	return InAddr.s_addr == Other.InAddr.s_addr;
}
bool Ip4::operator != (const Ip4 & Other) const
{
	return !(*this == Other);
}

Ip4::Ip4(const std::string & IpStr):
	InAddr(IpStrToInAddr(IpStr)), IpStr(IpStr)
{

}
Ip4::Ip4(const in_addr & InAddr):
	InAddr(InAddr), IpStr()
{

}

const std::string & Ip4::GetIpStr() const
{
	if(IpStr.empty())
		IpStr = InAddrToIpStr(InAddr);

	return IpStr;
}
const in_addr & Ip4::GetInAddr() const
{
	return InAddr;
}