#include "StringTools.hpp"

const char StringTools::Base64Table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

std::string StringTools::Base64Encode(const std::string & Input)
{
	const size_t InputLength = Input.length();
	const char * InputCStr = Input.c_str();
	size_t InputPos = 0;
	
	std::string Output;
	Output.reserve((InputLength / 3 + 1) * 4);
	
	while(InputPos < InputLength)
	{
		char InputChar[3];
		InputChar[0] = InputCStr[InputPos];
		InputChar[1] = (InputPos + 1 < InputLength) ? InputCStr[InputPos + 1] : 0;
		InputChar[2] = (InputPos + 2 < InputLength) ? InputCStr[InputPos + 2] : 0;
		
		char OutputChar[4];
		OutputChar[0] = Base64Table[InputChar[0] >> 2];
		OutputChar[1] = Base64Table[((InputChar[0] & 3) << 4) | InputChar[1] >> 4];
		OutputChar[2] = (InputPos + 1 < InputLength) ? Base64Table[((InputChar[1] & 15) << 2) | InputChar[2] >> 6] : '=';
		OutputChar[3] = (InputPos + 2 < InputLength) ? Base64Table[InputChar[2] & 63]: '=';
		
		Output.append(OutputChar, 4);
		
		InputPos += 3;
	}
	
	return Output;
}
std::string StringTools::ExtractFromBetween(const std::string & Source, const std::string & L, const std::string & R)
{
	const size_t LStartPos = Source.find(L);
	if(LStartPos == std::string::npos)
		return std::string();
	
	const size_t LEndPos = LStartPos + L.length();
	
	const size_t RStartPos = Source.find(R, LEndPos);
	if(RStartPos == std::string::npos)
		return std::string();
		
	return Source.substr(LEndPos, RStartPos - LEndPos);
}