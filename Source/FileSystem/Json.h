#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <vector>
class Json
{
public:
	Json(rapidjson::Document& document, rapidjson::Value& value);
	~Json();

	unsigned int Size() const;

	bool fromBuffer(char*& buffer);
	void toBuffer(rapidjson::StringBuffer& buffer);
	std::vector<const char*> GetVectorNames();

	template<typename T>
	Json operator[](const T* key) const;
	Json operator[](const unsigned index) const;

	operator bool() const;
	operator int() const;
	operator unsigned() const;
	operator float() const;
	operator long long() const;
	operator unsigned long long() const;
	operator double() const;
	operator std::string() const;

	void operator=(bool x);
	void operator=(int x);
	void operator=(unsigned x);
	void operator=(float x);
	void operator=(long long x);
	void operator=(unsigned long long x);
	void operator=(double x);
	void operator=(const char* x);

private:
	rapidjson::Document& document;
	rapidjson::Value& value;
};

inline unsigned int Json::Size() const
{
	return value.IsArray() ? value.Size() : 0;
}

inline Json::operator bool() const
{
	return value.IsBool() ? value.GetBool() : false;
}

inline Json::operator int() const
{
	return value.IsInt() ? value.GetInt() : 0;
}

inline Json::operator unsigned() const
{
	return value.IsUint() ? value.GetUint() : 0;
}

inline Json::operator float() const
{
	return value.IsDouble() ? value.GetFloat() : 0;
}

inline Json::operator long long() const
{
	return value.IsInt64() ? value.GetInt64() : 0;
}

inline Json::operator unsigned long long() const
{
	return value.IsUint64() ? value.GetUint64() : 0;
}

inline Json::operator double() const
{
	return value.IsDouble() ? value.GetDouble() : 0;
}

inline Json::operator std::string() const
{
	return value.IsString() ? value.GetString() : std::string();
}

template<typename T>
inline Json Json::operator[](const T* key) const
{
	if (!value.IsObject())
	{
		value.SetObject();
	}

	if (!value.HasMember(key))
	{
		value.AddMember(rapidjson::StringRef(key), rapidjson::Value(), document.GetAllocator());
	}

	return Json(document, value[key]);
}

inline void Json::operator=(bool x)
{
	value = x;
}

inline void Json::operator=(int x)
{
	value = x;
}

inline void Json::operator=(unsigned x)
{
	value = x;
}

inline void Json::operator=(float x)
{
	value = x;
}

inline void Json::operator=(long long x)
{
	value = x;
}

inline void Json::operator=(unsigned long long x)
{
	value = x;
}

inline void Json::operator=(double x)
{
	value = x;
}

inline void Json::operator=(const char* x)
{
	value.SetString(x, document.GetAllocator());
}