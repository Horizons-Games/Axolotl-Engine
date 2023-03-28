#include "Json.h"

#include "EngineLog.h"

#include "rapidjson/prettywriter.h"

Json::Json(rapidjson::Document& document, rapidjson::Value& value) : document(document), value(value) 
{
}

Json::~Json()
{
}

unsigned int Json::Size() const {
	return value.IsArray() ? value.Size() : 0;
}

bool Json::fromBuffer(char*& buffer)
{
	document.Parse<rapidjson::kParseNanAndInfFlag>(buffer);
	if (document.HasParseError()) 
	{
		ENGINE_LOG("Error parsing JSON: %s", rapidjson::GetParseErrorFunc(document.GetParseError()));
		return false;
	}
	return true;
}

void Json::toBuffer(rapidjson::StringBuffer& buffer)
{
	buffer.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, 
		rapidjson::CrtAllocator > writer(buffer);
	document.Accept(writer);
}

void Json::getVectorNames(std::vector<const char*>& vec)
{
	for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
		itr != document.MemberEnd(); ++itr)
	{
		vec.push_back(itr->name.GetString());		
	}
}

Json Json::operator[](unsigned index) const
{
	if (!value.IsArray())
	{
		value.SetArray();
	}

	while (index >= value.Size())
	{
		value.PushBack(rapidjson::Value(), document.GetAllocator());
	}

	return Json(document, value[index]);
}

Json::operator bool() const
{
	return value.IsBool() ? value.GetBool() : false;
}

Json::operator int() const
{
	return value.IsInt() ? value.GetInt() : 0;
}

Json::operator unsigned() const
{
	return value.IsUint() ? value.GetUint() : 0;
}

Json::operator float() const
{
	return value.IsDouble() ? value.GetFloat() : 0;
}

Json::operator long long() const
{
	return value.IsInt64() ? value.GetInt64() : 0;
}

Json::operator unsigned long long() const
{
	return value.IsUint64() ? value.GetUint64() : 0;
}

Json::operator double() const
{
	return value.IsDouble() ? value.GetDouble() : 0;
}

Json::operator std::string() const
{
	return value.IsString() ? value.GetString() : "";
}

