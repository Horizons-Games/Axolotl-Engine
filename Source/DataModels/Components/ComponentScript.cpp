#include "ComponentScript.h"

#include "EngineLog.h"

ComponentScript::ComponentScript(GameObject* owner) : Component(ComponentType::SCRIPT, true, owner, true)
{
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::RegisterField(const std::string& name, const std::function<std::any(void)>& getter, const std::function<void(const std::any&)> setter, FieldType type)
{
	switch (type)
	{
	case FieldType::FLOAT:
	{
		Field<float> floatField(name, std::any_cast<std::function<float(void)>>(getter), std::any_cast<std::function<void(const float&)>>(setter), type);
		members.push_back(floatField);
		break;
	}
	default:
	{
		ENGINE_LOG("Unknown field type");
		break;
	}
	}
}
