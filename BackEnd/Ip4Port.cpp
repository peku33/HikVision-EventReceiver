#include "Ip4Port.hpp"

#include <netinet/in.h>

std::string Ip4Port::ExtractIpFromIpPortStr(const std::string & IpPortStr)
{
	const size_t ColonPosition = IpPortStr.find(':');
	if(ColonPosition == std::string::npos)
		throw std::invalid_argument("Colon in IpPortStr not found (ExtractIpFromIpPortStr())");

	return IpPortStr.substr(0, ColonPosition);
}
uint16_t Ip4Port::ExtractPortFromIpPortStr(const std::string & IpPortStr)
{
	const size_t ColonPosition = IpPortStr.find(':');
	if(ColonPosition == std::string::npos)
		throw std::invalid_argument("Colon in IpPortStr not found (ExtractPortFromIpPortStr())");

	const std::string PortStr = IpPortStr.substr(ColonPosition + 1);
	if(PortStr.empty())
		throw std::invalid_argument("Empty port (ExtractPortFromIpPortStr())");

	size_t Idx = std::string::npos;
	uint16_t Port = stoi(PortStr, &Idx);

	if(Idx != PortStr.length())
		throw std::invalid_argument("Invalid port (ExtractPortFromIpPortStr())");

	return Port;
}

Ip4Port::Ip4Port(const Ip4 & Ip, const uint16_t Port):
	Ip(Ip), Port(Port),
	IpPortStr(), SockAddrIn()
{

}
Ip4Port::Ip4Port(const std::string & IpPortStr):
	Ip(ExtractIpFromIpPortStr(IpPortStr)), Port(ExtractPortFromIpPortStr(IpPortStr)),
	IpPortStr(IpPortStr), SockAddrIn()
{

}
Ip4Port::Ip4Port(const sockaddr_in & SockAddrIn):
	Ip(SockAddrIn.sin_addr), Port(ntohs(SockAddrIn.sin_port)),
	IpPortStr(), SockAddrIn(SockAddrIn)
{
	if(SockAddrIn.sin_family != AF_INET)
		throw std::invalid_argument("sin_family != AF_INET (Ip4Port::Ip4Port())");
}

bool Ip4Port::operator == (const Ip4Port & Other) const
{
	return Ip == Other.Ip && Port == Other.Port;
}
bool Ip4Port::operator != (const Ip4Port & Other) const
{
	return !(*this == Other);
}

const Ip4 & Ip4Port::GetIp() const
{
	return Ip;
}
uint16_t Ip4Port::GetPort() const
{
	return Port;
}

const std::string & Ip4Port::GetIpPortStr() const
{
	if(IpPortStr.empty())
		IpPortStr = Ip.GetIpStr() + ":" + std::to_string(Port);

	return IpPortStr;
}
const sockaddr_in & Ip4Port::GetSockAddrIn() const
{
	if(!SockAddrIn)
	{
		sockaddr_in _SockAddrIn;
		_SockAddrIn.sin_family = AF_INET;
		_SockAddrIn.sin_port = htons(Port);
		_SockAddrIn.sin_addr = Ip.GetInAddr();

		SockAddrIn = _SockAddrIn;
	}

	return *SockAddrIn;
}