#pragma once

#include <string>

#include "NoCreate.hpp"

class StringTools : public NoCreate
{
	//Base64
	private:
		static const char Base64Table[];

	public:
		static std::string Base64Encode(const std::string & Input);

	//Other Tools
	public:
		static std::string ExtractFromBetween(const std::string & Source, const std::string & L, const std::string & R);
};