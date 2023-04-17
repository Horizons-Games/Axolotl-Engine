#include "Json.h"

#include "EngineLog.h"

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


