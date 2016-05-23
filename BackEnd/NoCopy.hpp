#pragma once

/**
 * @brief Base class used for preventing copy-construction, assignment or move-construction
 */
class NoCopy
{
	protected:
		NoCopy() = default;

	public:
		NoCopy(const NoCopy &) = delete;
		NoCopy & operator = (const NoCopy &) = delete;

		NoCopy(NoCopy &&) = delete;
		NoCopy & operator = (NoCopy &&) = delete;
};