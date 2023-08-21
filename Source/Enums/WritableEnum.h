#pragma once

#include <algorithm>

namespace
{
template<typename E>
std::string ToString(E)
{
	static_assert(false,
				  "No ToString overload defined for this type, remember to declare it using WRITABLE_ENUM macro");
}

template<typename E>
E FromString(const std::string&)
{
	static_assert(false,
				  "No FromString overload defined for this type, remember to declare it using WRITABLE_ENUM macro");
}
} // namespace

#define WRITABLE_ENUM(enumName, ...)                                        \
	enum class enumName                                                     \
	{                                                                       \
		__VA_ARGS__,                                                        \
		UNKNOWN                                                             \
	};                                                                      \
	namespace                                                               \
	{                                                                       \
	template<>                                                              \
	std::string ToString(enumName enumValue)                                \
	{                                                                       \
		return axo::detail::ToString(enumValue, #__VA_ARGS__);              \
	}                                                                       \
	template<>                                                              \
	enumName FromString(const std::string& enumString)                      \
	{                                                                       \
		return axo::detail::FromString<enumName>(enumString, #__VA_ARGS__); \
	}                                                                       \
	}

namespace axo::detail
{
std::vector<std::string> FormatNamesVector(std::string&& names);

template<typename E>
std::string ToString(E enumValue, std::string&& names)
{
	std::vector<std::string> namesVector = FormatNamesVector(std::move(names));

	return namesVector[static_cast<int>(enumValue)];
}

template<typename E>
E FromString(const std::string& enumString, std::string&& names)
{
	std::vector<std::string> namesVector = FormatNamesVector(std::move(names));

	std::string stringToLower = enumString;
	std::transform(stringToLower.begin(), stringToLower.end(), stringToLower.begin(), ::tolower);

	for (int i = 0; i < namesVector.size(); ++i)
	{
		std::string nameToLower = namesVector[i];
		std::transform(nameToLower.begin(), nameToLower.end(), nameToLower.begin(), ::tolower);
		if (stringToLower == nameToLower)
		{
			return static_cast<E>(i);
		}
	}
	return E::UNKNOWN;
}
} // namespace axo::detail
