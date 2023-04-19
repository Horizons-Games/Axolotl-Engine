#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), sentence("Horizons")
{
	REGISTER_FIELD(Value, float);
	REGISTER_FIELD(Sentence, std::string);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("%f", value);
	ENGINE_LOG("%s", sentence.c_str());
}

float DefaultScript::GetValue() const
{
	return value;
}

void DefaultScript::SetValue(float value)
{
	this->value = value;
}

std::string DefaultScript::GetSentence() const
{
	return sentence;
}

void DefaultScript::SetSentence(const std::string sentence)
{
	this->sentence = sentence;
}