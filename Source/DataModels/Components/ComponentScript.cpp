#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "Scripting/Script.h"

#include "FileSystem/Json.h"

ComponentScript::ComponentScript(bool active, GameObject* owner) : 
	Component(ComponentType::SCRIPT, active, owner, true), script(nullptr)
{
}

//ComponentScript::ComponentScript(const ComponentScript& component) :
//	Component(component), script(std::unique_ptr<Script>(new Script(component.script)))
//{
//}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::Init()
{
	script->Init();
}

void ComponentScript::Start()
{
	script->Start();
}

void ComponentScript::PreUpdate()
{
	script->PreUpdate(App->GetDeltaTime() /*should be OnPlayTime*/);
}

void ComponentScript::Update()
{
	script->Update(App->GetDeltaTime() /*should be OnPlayTime*/);
}

void ComponentScript::PostUpdate()
{
	script->PostUpdate(App->GetDeltaTime() /*should be OnPlayTime*/);
}

void ComponentScript::CleanUp()
{
	script->CleanUp();
}

void ComponentScript::SaveOptions(Json& meta)
{
	// Save serialize values of Script
	meta["constructName"] = this->constructName.c_str();
}

void ComponentScript::LoadOptions(Json& meta)
{
	// Load serialize values of Script
	this->constructName = meta["constructName"];
}
