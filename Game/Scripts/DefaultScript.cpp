#include "DefaultScript.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10)
{
	REGISTER_FIELD(Value, float, FLOAT);
	REGISTER_FIELD(Value2, float, FLOAT);
}

void DefaultScript::Update(float deltaTime)
{
	ENGINE_LOG("%f", value);
}
