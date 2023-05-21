#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("{}", value);
	ENGINE_LOG("{}", sentence);

	if (character != nullptr)
	{
		ENGINE_LOG("{}", character)
	}

	ENGINE_LOG("{}", check);
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
