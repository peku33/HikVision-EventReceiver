#include "CircularExtractBuffer.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	CircularExtractBuffer A(128, "<a>", "</a>");

	A.Append("|PRESTART|<a> </a>|POSTEND|");
	assert(A.Dump() == "|PRESTART|<a> </a>|POSTEND|");
	assert(A.Extract() == "<a> </a>");
	assert(A.Dump() == "|POSTEND|");

	A.Append("<a>|DATA_A|");
	assert(A.Dump() == "|POSTEND|<a>|DATA_A|");
	assert(A.Extract() == std::string());
	assert(A.Dump() == "|POSTEND|<a>|DATA_A|");

	assert(A.Dump() == "|POSTEND|<a>|DATA_A|");
	A.Append("|DATA_B|");
	assert(A.Dump() == "|POSTEND|<a>|DATA_A||DATA_B|");
	A.Append("|DATA_C|</a>");
	assert(A.Dump() == "|POSTEND|<a>|DATA_A||DATA_B||DATA_C|</a>");
	assert(A.Extract() == "<a>|DATA_A||DATA_B||DATA_C|</a>");

	assert(A.Dump() == std::string());

	return 0;
}