#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "Scripting/Script.h"
#include "Scripting/ScriptFactory.h"

#include "FileSystem/Json.h"

ComponentScript::ComponentScript(bool active, GameObject* owner) : 
	Component(ComponentType::SCRIPT, active, owner, true), script(nullptr)
{
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::Init()
{
	if (script)
	{
		script->Init();
	}
}

void ComponentScript::Start()
{
	if (script)
	{
		script->Start();
	}
}

void ComponentScript::PreUpdate()
{
	if (script)
	{
		script->PreUpdate(App->GetDeltaTime() /*should be OnPlayTime*/);
	}
}

void ComponentScript::Update()
{
	if (script)
	{
		script->Update(App->GetDeltaTime() /*should be OnPlayTime*/);
	}
}

void ComponentScript::PostUpdate()
{
	if (script)
	{
		script->PostUpdate(App->GetDeltaTime() /*should be OnPlayTime*/);
	}
}

void ComponentScript::CleanUp()
{
	if (script)
	{
		script->CleanUp();
	}
}

void ComponentScript::SaveOptions(Json& meta)
{
	// Save serialize values of Script
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);
	meta["constructName"] = this->constructName.c_str();
	Json fields = meta["fields"];
	int index = 0;
	for (TypeFieldPair enumAndValue : script->GetFields())
	{
		Json field = fields[index];
		FieldType type = enumAndValue.first;
		switch (type)
		{
			case FieldType::FLOAT:
			{
				field["name"] = std::get<Field<float>>(enumAndValue.second).name.c_str();
				field["value"] = std::get<Field<float>>(enumAndValue.second).getter();
				field["type"] = static_cast<int>(enumAndValue.first);
				break;
			}
			case FieldType::STRING:
			{
				field["name"] = std::get<Field<std::string>>(enumAndValue.second).name.c_str();
				field["value"] = std::get<Field<std::string>>(enumAndValue.second).getter().c_str();
				field["type"] = static_cast<int>(enumAndValue.first);
				break;
			}
			default:
				break;
		}
		++index;
	}
}

void ComponentScript::LoadOptions(Json& meta)
{
	// Load serialize values of Script
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	constructName = meta["constructName"];
	script = App->scriptFactory->ConstructScript(constructName.c_str());
	script->SetApplication(App.get());
	script->SetGameObject(owner);
	Json fields = meta["fields"];
	for (unsigned int i = 0; i < fields.Size(); ++i)
	{
		Json field = fields[i];
		FieldType fieldType = static_cast<FieldType>(static_cast<int>(field["type"]));
		switch (fieldType)
		{
			case FieldType::FLOAT:
			{
				std::string valueName = field["name"];
				std::optional<Field<float>> optField = script->GetField<float>(valueName);
				if (optField)
				{
					optField.value().setter(field["value"]);
				}
				break;
			}

			case FieldType::STRING:
			{
				std::string valueName = field["name"];
				std::optional<Field<std::string>> optField = script->GetField<std::string>(valueName);
				if (optField)
				{
					optField.value().setter(field["value"]);
				}
				break;
			}

			default:
				break;
		}
	}
}
