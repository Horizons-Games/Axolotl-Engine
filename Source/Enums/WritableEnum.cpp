#include "StdAfx.h"
#include "WritableEnum.h"

namespace axo::detail
{
std::vector<std::string> FormatNamesVector(std::string&& names)
{
	std::vector<std::string> namesVector;

	std::stringstream ss(names);
	std::string name;

	while (ss >> name)
	{
		size_t pos = name.find(',');
		if (pos != std::string::npos)
		{
			namesVector.push_back(name.substr(0, pos));
		}
		else
		{
			namesVector.push_back(name);
		}
	}

	return std::move(namesVector);
}
} // namespace axo::detail
