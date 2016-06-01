#include "../StringTools.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	assert(StringTools::ExtractFromBetween("<a>|TEXT|</a>", "<a>", "</a>") == "|TEXT|");
	assert(StringTools::ExtractFromBetween("<a><a>|TEXT|</a>", "<a>", "</a>") == "<a>|TEXT|");
	assert(StringTools::ExtractFromBetween("<a><a>|TEXT|</a></a>", "<a>", "</a>") == "<a>|TEXT|");
	assert(StringTools::ExtractFromBetween("<a>|TEXT|</a></a>", "<a>", "</a>") == "|TEXT|");
	assert(StringTools::ExtractFromBetween("<a></a></a>", "<a>", "</a>") == "");
	assert(StringTools::ExtractFromBetween("<a></a>", "<a>", "</a>") == "");
	assert(StringTools::ExtractFromBetween("|PRE|<a>|TEXT|</a>|POST|", "<a>", "</a>") == "|TEXT|");

	return 0;
}