#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vec3(2.1f,1.2f,1.4f), sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Vector3, float3);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("%f", value);
	ENGINE_LOG("%f %f %f ", vec3[2], vec3[1], vec3[0]);
	ENGINE_LOG("%s", sentence.c_str());

	if (character != nullptr)
	{
		ENGINE_LOG("%s", character->GetName().c_str())
	}

	ENGINE_LOG("%s", std::to_string(check).c_str());
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
	ENGINE_LOG("My Character has been changed!");
	this->character = character;
}
