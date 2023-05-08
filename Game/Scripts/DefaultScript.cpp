#include "DefaultScript.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vec3(2.1,1.2,1.4), sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(Value, float);
	REGISTER_FIELD(Vector3, float3);
	REGISTER_FIELD(Sentence, std::string);
	REGISTER_FIELD(Character, GameObject*);
	REGISTER_FIELD(Check, bool);
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

float DefaultScript::GetValue() const
{
	return value;
}

void DefaultScript::SetValue(float value)
{
	this->value = value;
}

float3 DefaultScript::GetVector3() const
{
	return vec3;
}

void DefaultScript::SetVector3(float3 vec3)
{
	this->vec3 = vec3;
}

std::string DefaultScript::GetSentence() const
{
	return sentence;
}

void DefaultScript::SetSentence(const std::string& sentence)
{
	this->sentence = sentence;
}

GameObject* DefaultScript::GetCharacter() const
{
	return character;
}

void DefaultScript::SetCharacter(GameObject* character)
{
	this->character = character;
}

bool DefaultScript::GetCheck() const
{
	return check;
}

void DefaultScript::SetCheck(bool check)
{
	this->check = check;
}