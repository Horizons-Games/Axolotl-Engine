#include "DefaultScript.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/ComponentScript.h"
#include "../Scripts/HelloWorldScript.h"

#include "AxoLog.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vecStr{ "Esto", "es un", "vector string", "de prueba" }, vec{ 1.2f,2.3f,3.3f }, vec3(2.1f, 1.2f, 1.4f), sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Vector3, float3);
	REGISTER_FIELD_WITH_ACCESSORS(VectorFloat, std::vector<float>);
	REGISTER_FIELD(vecStr, std::vector<std::string>);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
	REGISTER_FIELD(transform, ComponentTransform*);
	REGISTER_FIELD_WITH_ACCESSORS(Script, HelloWorldScript*);
}

void DefaultScript::Update(float deltaTime)
{
	LOG_DEBUG("{}", value);
	LOG_DEBUG("{} {} {} ", vec3[2], vec3[1], vec3[0]);
	LOG_DEBUG("{}", sentence.c_str());

	if (character != nullptr)
	{
		LOG_DEBUG("{}", character);
	}

	LOG_DEBUG("{}", check);
}

const float3& DefaultScript::GetVector3() const
{
	return vec3;
}

void DefaultScript::SetVector3(const float3& vec3)
{
	this->vec3 = vec3;
}

const std::vector<float>& DefaultScript::GetVectorFloat() const
{
	return vec;
}

void DefaultScript::SetVectorFloat(const std::vector<float>& vec)
{
	this->vec = vec;
}

const std::vector<std::string>& DefaultScript::GetVectorStr() const
{
	return vecStr;
}

void DefaultScript::SetVectorStr(const std::vector<std::string>& vec)
{
	this->vecStr = vec;
}

GameObject* DefaultScript::GetCharacter() const
{
	return character;
}

void DefaultScript::SetCharacter(GameObject* character)
{
	LOG_INFO("My Character has been changed!");
	this->character = character;
}

HelloWorldScript* DefaultScript::GetScript() const
{
	return script;
}

void DefaultScript::SetScript(HelloWorldScript* script)
{
	this->script = script;
}
