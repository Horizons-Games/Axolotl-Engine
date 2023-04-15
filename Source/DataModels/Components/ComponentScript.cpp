#include "ComponentScript.h"

#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentScript::ComponentScript(bool active, GameObject* owner) : 
	Component(ComponentType::SCRIPT, active, owner, true)
{
	this->initialized = false;
}

ComponentScript::~ComponentScript()
{
}
void ComponentScript::SaveOptions(Json& meta)
{
	// Save serialize values of Script
	meta["constructName"] = this->constructName;
}

void ComponentScript::LoadOptions(Json& meta)
{
	// Load serialize values of Script
	this->constructName = ((std::string)meta["constructName"]).c_str();
}
