#include "CircularExtractBuffer.hpp"

#include <stdexcept>

CircularExtractBuffer::CircularExtractBuffer(const size_t Capacity, const std::string & Start, const std::string & End):
	Start(Start), End(End), Buffer(Capacity)
{

}
void CircularExtractBuffer::Append(const std::string & Data)
{
	if(Buffer.capacity() - Buffer.size() < Data.length())
		throw std::overflow_error("Buffer.Capacity() - Buffer.size() < Data.length() (CircularExtractBuffer::Append())");

	Buffer.insert(Buffer.end(), Data.cbegin(), Data.cend());
}
std::string CircularExtractBuffer::Extract()
{
	if(Buffer.size() < Start.length() + End.length())
		return std::string();

	const decltype(Buffer)::iterator StartStart = std::search(Buffer.begin(), Buffer.end(), Start.cbegin(), Start.cend());
	if(StartStart == Buffer.end())
		return std::string();

	const decltype(Buffer)::reverse_iterator EndSearchIterator(StartStart + Start.length());
	const decltype(Buffer)::reverse_iterator EndEnd = std::search(Buffer.rbegin(), EndSearchIterator, End.crbegin(), End.crend());
	if(EndEnd == EndSearchIterator)
		return std::string();

	const std::string Result(StartStart, EndEnd.base());

	Buffer.erase_begin(EndEnd.base() - Buffer.begin());

	return Result;
}
#ifdef DEBUG
	std::string CircularExtractBuffer::Dump() const
	{
		return std::string(Buffer.begin(), Buffer.end());
	}
#endif