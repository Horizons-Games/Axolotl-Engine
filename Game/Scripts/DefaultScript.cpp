#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vec3(2.1f, 1.2f, 1.4f), sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Vector3, float3);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
}

void DefaultScript::Update(float deltaTime)
{
	LOG_INFO("%f", value);
	LOG_INFO("%f %f %f ", vec3[2], vec3[1], vec3[0]);
	LOG_INFO("%s", sentence.c_str());

	if (character != nullptr)
	{
		LOG_INFO("{}", character)
	}

	LOG_INFO("{}", check);
}

const float3& DefaultScript::GetVector3() const
{
	return vec3;
}

void DefaultScript::SetVector3(const float3& vec3)
{
	this->vec3 = vec3;
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
