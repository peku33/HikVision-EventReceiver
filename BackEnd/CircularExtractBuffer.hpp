#pragma once

#include <boost/circular_buffer.hpp>

/**
 * @brief Buffer used to accumulate incoming data and extract full packet. Packet is recognized as data between first Start and first (after Start) End
 */
class CircularExtractBuffer
{
	public:
		/**
		 * @brief Constructor. Initizlizes empty buffer, stores Start and End in internat storage
		 * 
		 * @param Capacity Maximum accumulator length. Total data put by Append() may not exceed Capacity, otherwise overflow_error would be thrown on Append()
		 * @param Start String marking start of packet
		 * @param End String marking end of packet
		 */
		CircularExtractBuffer(const size_t Capacity, const std::string & Start, const std::string & End);

	private:
		const std::string Start;
		const std::string End;

	private:
		boost::circular_buffer<char> Buffer;

	public:
		/**
		 * @brief Appends data to internal buffer
		 * 
		 * @param Data Data to be appended
		 */
		void Append(const std::string & Data);

		/**
		 * @brief Tries to extract packet
		 * @details Searches for first Start, and first End after Start. If found - returns data from between Start and End and deletes all data from beggining to last character of End. Content after End is not discarded.
		 * @return String between Start and End or std::string() (empty string) if Start / End was not found
		 */
		std::string Extract();

	#ifdef DEBUG
		public:
			std::string Dump() const;
	#endif
};