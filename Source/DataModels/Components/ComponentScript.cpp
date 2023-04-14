#include "ComponentScript.h"

#include "EngineLog.h"

ComponentScript::ComponentScript(GameObject* owner) : Component(ComponentType::SCRIPT, true, owner, true)
{
	REGISTER_FIELD(Test, float, FLOAT);
}

ComponentScript::~ComponentScript()
{
}
