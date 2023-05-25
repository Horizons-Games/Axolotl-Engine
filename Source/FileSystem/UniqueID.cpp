#include "UniqueID.h"
#include <sstream>

// For using only include UniqueID.h and add UID uid = UniqueID::GenerateUID(); to create a number UID
// and std::string uid = UniqueID::GenerateUUID(); to create a number+letter UID

UniqueID::~UniqueID()
{
}

std::string UniqueID::GenerateUUID()
{
	std::stringstream ss; // read/write the string
	ss << std::hex;
	ss << std::uppercase;

	int i;

	for (i = 0; i < 8; ++i)
	{
		ss << uuidNew(creator);
	}
	ss << "-";
	for (i = 0; i < 4; ++i)
	{
		ss << uuidNew(creator);
	}
	ss << "-";

	for (i = 0; i < 3; ++i)
	{
		ss << uuidNew(creator);
	}
	ss << "-";

	for (i = 0; i < 3; ++i)
	{
		ss << uuidNew(creator);
	}
	ss << "-";
	for (i = 0; i < 12; ++i)
	{
		ss << uuidNew(creator);
	}
	return ss.str();
}