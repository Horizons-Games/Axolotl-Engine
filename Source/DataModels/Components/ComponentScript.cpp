#include "ComponentScript.h"

#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentScript::ComponentScript(bool active, GameObject* owner)
	: Component(ComponentType::PLAYER, active, owner, true)
{
}

ComponentScript::~ComponentScript()
{
}



void ComponentScript::SaveOptions(Json& meta)
{
}

void ComponentScript::LoadOptions(Json& meta)
{
}
