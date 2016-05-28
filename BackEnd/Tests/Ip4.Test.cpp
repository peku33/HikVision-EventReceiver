#include "../Ip4.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	Ip4 a("1.2.3.4");
	std::cout << a.GetIpStr() << std::endl;

	Ip4 b(a.GetInAddr());
	std::cout << b.GetIpStr() << std::endl;

	assert(a == b);
	assert(a.GetIpStr() == b.GetIpStr());
	assert(a.GetInAddr().s_addr == b.GetInAddr().s_addr);

	return 0;
}