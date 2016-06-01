#include "../Ip4Port.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	const std::string ValidIpPort = "1.2.3.4:1234";

	Ip4Port a(ValidIpPort);
	Ip4Port b(a.GetIp(), a.GetPort());
	Ip4Port c(b.GetIpPortStr());
	Ip4Port d(c.GetSockAddrIn());
	Ip4Port e(d);

	for(const Ip4Port & I : {a, b, c, d, e})
	{
		assert(a == I);
		assert(a.GetIp() == I.GetIp());
		assert(a.GetPort() == I.GetPort());
		assert(a.GetIpPortStr() == I.GetIpPortStr());

		std::cout << "-> " << I.GetIp().GetIpStr() << " " << I.GetPort() << " " << I.GetIpPortStr() << std::endl;
	}

	return 0;
}