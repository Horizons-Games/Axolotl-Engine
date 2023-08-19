#pragma once

class GameObject;
class Resource;

namespace axo
{
namespace detail
{
size_t FindReplaceToken(const std::string& formatString);

bool Format(std::string& format, int arg);
bool Format(std::string& format, unsigned int arg);
bool Format(std::string& format, float arg);
bool Format(std::string& format, const char* arg);
bool Format(std::string& format, const std::string& arg);
bool Format(std::string& format, bool arg);
bool Format(std::string& format, const GameObject* arg);
bool Format(std::string& format, unsigned long long arg);
// since it's only for logging purposes, pass it as reference to avoid increasing and decreasing ref count
bool Format(std::string& format, const std::shared_ptr<Resource>& arg);
// for glew logging
bool Format(std::string& format, const unsigned char* arg);
} // namespace detail

template<typename... Args>
std::string Format(const std::string& format, Args&&... args)
{
	std::string formattedString = format;
	// this iterates through the variadic argument pack like a loop
	(
		[&]()
		{
			assert(detail::Format(formattedString, args) && "Too many arguments in log call!");
		}(),
		...);

	assert(detail::FindReplaceToken(formattedString) == std::string::npos && "Too few arguments in log call!");

	return formattedString;
}

} // namespace axo
