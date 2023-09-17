#include "StdAfx.h"

#include "Formatter.h"

#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Resources/Resource.h"

namespace axo::detail
{
namespace
{
const char* replaceToken = "{}";
}

size_t FindReplaceToken(const std::string& formatString)
{
	return formatString.find(replaceToken);
}

bool Format(std::string& format, int arg)
{
	return Format(format, std::to_string(arg));
}

bool Format(std::string& format, unsigned int arg)
{
	return Format(format, std::to_string(arg));
}

bool Format(std::string& format, float arg)
{
	return Format(format, std::to_string(arg));
}

bool Format(std::string& format, const char* arg)
{
	return Format(format, std::string(arg));
}

bool Format(std::string& format, const std::string& arg)
{
	size_t firstToken = FindReplaceToken(format);
	if (firstToken != std::string::npos)
	{
		format.replace(firstToken, strlen(replaceToken), arg);
		return true;
	}
	return false;
}

bool Format(std::string& format, bool arg)
{
	return Format(format, std::to_string(arg));
}

bool Format(std::string& format, const GameObject* arg)
{
	return Format(format, arg != nullptr ? arg->GetName() : "NULL");
}

bool Format(std::string& format, unsigned long long arg)
{
	return Format(format, std::to_string(arg));
}

bool Format(std::string& format, const std::shared_ptr<Resource>& arg)
{
	if (arg)
	{
		return Format(format, arg->GetUID());
	}
	return Format(format, "NULL");
}

bool Format(std::string& format, const unsigned char* arg)
{
	return Format(format, reinterpret_cast<const char*>(arg));
}

} // namespace axo::detail
