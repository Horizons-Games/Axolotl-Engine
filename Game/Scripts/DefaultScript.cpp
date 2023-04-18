#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10)
{
	REGISTER_FIELD(Value, float);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("%f", value);
}

float DefaultScript::GetValue() const
{
	return value;
}

void DefaultScript::SetValue(float value)
{
	this->value = value;
}
