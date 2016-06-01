#pragma once

#include "NoCopy.hpp"

/**
 * @brief Base class preventing derived to be instanced. Used to create namespace-like classes (static methods / variables only)
 */
class NoCreate : public NoCopy
{
	public:
		NoCreate() = delete;	
};