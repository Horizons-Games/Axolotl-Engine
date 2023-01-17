#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <string>

class Json 
{
public:
	Json(rapidjson::Document& document, rapidjson::Value& value);

	unsigned int Size() const;

	bool fromBuffer(char*& buffer);
	void toBuffer(rapidjson::StringBuffer& buffer);

	template<typename T> Json operator[](const T* key) const;
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