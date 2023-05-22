#include "Json.h"

#include "AxoLog.h"

#include "rapidjson/prettywriter.h"

Json::Json(rapidjson::Document& document, rapidjson::Value& value) : document(document), value(value)
{
}

Json::~Json()
{
}

bool Json::fromBuffer(char*& buffer)
{
	document.Parse<rapidjson::kParseNanAndInfFlag>(buffer);
	if (document.HasParseError())
	{
		LOG_INFO("Error parsing JSON: {}", rapidjson::GetParseErrorFunc(document.GetParseError()));
		return false;
	}
	return true;
}

void Json::toBuffer(rapidjson::StringBuffer& buffer)
{
	buffer.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator>
		writer(buffer);
	document.Accept(writer);
}

std::vector<const char*> Json::GetVectorNames()
{
	std::vector<const char*> vec;
	for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
	{
		vec.push_back(itr->name.GetString());
	}
	return vec;
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
