#pragma once

#include <string>

namespace
{
std::string ConvertU8String(const std::u8string& string)
{
	return std::string(std::begin(string), std::end(string));
}
} // namespace
