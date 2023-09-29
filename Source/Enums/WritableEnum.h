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

#define WRITABLE_ENUM(enumName, ...)                                                              \
	enum class enumName                                                                           \
	{                                                                                             \
		__VA_ARGS__,                                                                              \
		UNKNOWN                                                                                   \
	};                                                                                            \
                                                                                                  \
	namespace axo::detail::enumName                                                               \
	{                                                                                             \
	std::vector<std::string> namesVector = FormatNamesVector(#__VA_ARGS__);                       \
	}                                                                                             \
                                                                                                  \
	namespace                                                                                     \
	{                                                                                             \
	template<>                                                                                    \
	std::string ToString(enumName enumValue)                                                      \
	{                                                                                             \
		return axo::detail::ToString(enumValue, axo::detail::enumName::namesVector);              \
	}                                                                                             \
                                                                                                  \
	template<>                                                                                    \
	enumName FromString(const std::string& enumString)                                            \
	{                                                                                             \
		return axo::detail::FromString<enumName>(enumString, axo::detail::enumName::namesVector); \
	}                                                                                             \
	}

namespace axo::detail
{
std::vector<std::string> FormatNamesVector(std::string&& names);

template<typename E>
std::string ToString(E enumValue, const std::vector<std::string>& namesVector)
{
	return namesVector[static_cast<int>(enumValue)];
}

template<typename E>
E FromString(const std::string& enumString, const std::vector<std::string>& namesVector)
{
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
