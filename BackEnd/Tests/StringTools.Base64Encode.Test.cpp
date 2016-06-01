#include "StringTools.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	assert(StringTools::Base64Encode("") == "");
	assert(StringTools::Base64Encode("A") == "QQ==");
	assert(StringTools::Base64Encode("-") == "LQ==");
	assert(StringTools::Base64Encode("This is Test _ !@#$%^&*()") == "VGhpcyBpcyBUZXN0IF8gIUAjJCVeJiooKQ==");

	return 0;
}