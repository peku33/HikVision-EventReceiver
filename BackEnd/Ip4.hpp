#pragma once

#include <string>
#include <netinet/in.h>

/**
 * @brief Class representing Ip4 address (without port)
 * @details It's just a String <> in_addr converter with some additional methods. Basic representation of ip in this class relays on in_addr structure.
 */
class Ip4
{
	//Standalone converters
	private:
		/**
		 * @brief Takes ip4 in_addr and converts to dotted x.x.x.x string
		 * @details Throws exception on failure
		 * 
		 * @param InAddr Input in_addr
		 * @return Dotted ip4 notation (x.x.x.x)
		 */
		static std::string InAddrToIpStr(const in_addr & InAddr);

		/**
		 * @brief Converts string dotted representation of a ip4 address to in_addr
		 * @details Throw exception on failure
		 * 
		 * @param IpStr Input dotted string to be converted to in_addr
		 * @return in_addr representing the same ip4 address as IpStr
		 */
		static in_addr IpStrToInAddr(const std::string & IpStr);

	public:
		/**
		 * @brief Construstor, constructs from dotted string (see IpStrToInAddr())
		 * 
		 * @param IpStr Dotted representation of ip4 (x.x.x.x)
		 */
		Ip4(const std::string & IpStr);

		/**
		 * @brief Constructor, constructs from in_addr (see InAddrToIpStr())
		 * 
		 * @param InAddr in_addr
		 */
		Ip4(const in_addr & InAddr);

		//Dont have to comment it...
		bool operator == (const Ip4 & Other) const;
		bool operator != (const Ip4 & Other) const;

	private:
		in_addr InAddr;

	private:
		//Lazy-initialized cache. GetIpStr() fills it with first use. GetIpStr() is const, so we must make this mutable
		mutable std::string IpStr;

	public:
		/**
		 * @brief Returns dotted string representation of ip4. There is internal lazy-initialized cache, so first call to this function *MAY* perform conversion. Later it would use cache
		 * @return Dotted representation (x.x.x.x)
		 */
		const std::string & GetIpStr() const;

		/**
		 * @brief Returns objects in_addr
		 * @return in_addr represented by this object
		 */
		const in_addr & GetInAddr() const;
};