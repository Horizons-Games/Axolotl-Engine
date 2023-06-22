#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "Scripting/Script.h"
#include "Scripting/ScriptFactory.h"

#include "FileSystem/Json.h"

#include "Math/float3.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "ComponentRigidBody.h"

namespace
{
template<typename T>
std::optional<Field<T>> GetField(IScript* script, const std::string& name)
{
	for (const TypeFieldPair& enumAndType : script->GetFields())
	{
		if (TypeToEnum<T>::value == enumAndType.first)
		{
			Field<T> field = std::get<Field<T>>(enumAndType.second);
			if (field.name == name)
			{
				return field;
			}
		}
	}
	return std::nullopt;
}
}

ComponentScript::ComponentScript(bool active, GameObject* owner) :
	Component(ComponentType::SCRIPT, active, owner, true),
	script(nullptr)
{
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::Init()
{
	if (IsEnabled() && script)
	{
		script->Init();
	}
}

void ComponentScript::Start()
{
	if (IsEnabled() && script)
	{
		script->Start();
	}
}

void ComponentScript::PreUpdate()
{
	if (IsEnabled() && script && !App->GetScriptFactory()->IsCompiling())
	{
		script->PreUpdate(App->GetDeltaTime());
	}
}

void ComponentScript::Update()
{
	if (IsEnabled() && script && !App->GetScriptFactory()->IsCompiling())
	{
		script->Update(App->GetDeltaTime());
	}
}

void ComponentScript::PostUpdate()
{
	if (IsEnabled() && script && !App->GetScriptFactory()->IsCompiling())
	{
		script->PostUpdate(App->GetDeltaTime());
	}
}

void ComponentScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (IsEnabled() && script && !App->GetScriptFactory()->IsCompiling())
	{
		script->OnCollisionEnter(other);
	}

}
void ComponentScript::OnCollisionExit(ComponentRigidBody* other)
{
	if (IsEnabled() && script && !App->GetScriptFactory()->IsCompiling())
	{
		script->OnCollisionExit(other);
	}
}



void ComponentScript::CleanUp()
{
	if (IsEnabled() && script)
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

	if (script == nullptr)
	{
		return;
	}

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
			case FieldType::VECTOR3:
			{
				Field<float3> fieldInstance = std::get<Field<float3>>(enumAndValue.second);
				field["name"] = fieldInstance.name.c_str();
				float3 fieldValue = fieldInstance.getter();
				field["value x"] = fieldValue[0];
				field["value y"] = fieldValue[1];
				field["value z"] = fieldValue[2];
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

			case FieldType::GAMEOBJECT:
			{
				field["name"] = std::get<Field<GameObject*>>(enumAndValue.second).name.c_str();

				if (std::get<Field<GameObject*>>(enumAndValue.second).getter() != nullptr)
				{
					field["value"] = std::get<Field<GameObject*>>(enumAndValue.second).getter()->GetUID();
				}
				else
				{
					field["value"] = 0;
				}

				field["type"] = static_cast<int>(enumAndValue.first);
				break;
			}

			case FieldType::BOOLEAN:
			{
				field["name"] = std::get<Field<bool>>(enumAndValue.second).name.c_str();
				field["value"] = std::get<Field<bool>>(enumAndValue.second).getter();
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
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
	constructName = meta["constructName"];
	script = App->GetScriptFactory()->ConstructScript(constructName.c_str());

	if (script == nullptr)
	{
		return;
	}

	script->SetOwner(owner);
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
				std::optional<Field<float>> optField = GetField<float>(script, valueName);
				if (optField)
				{
					optField.value().setter(field["value"]);
				}
				break;
			}
			case FieldType::VECTOR3:
			{
				std::string valueName = field["name"];
				std::optional<Field<float3>> optField = GetField<float3>(script, valueName);
				if (optField)
				{
					float3 vec3(field["value x"], field["value y"], field["value z"]);
					optField.value().setter(vec3);
				}
				break;
			}

			case FieldType::STRING:
			{
				std::string valueName = field["name"];
				std::optional<Field<std::string>> optField = GetField<std::string>(script, valueName);
				if (optField)
				{
					optField.value().setter(field["value"]);
				}
				break;
			}

			case FieldType::GAMEOBJECT:
			{
				std::string valueName = field["name"];
				std::optional<Field<GameObject*>> optField = GetField<GameObject*>(script, valueName);
				if (optField)
				{
					UID fieldUID = field["value"];
					if (fieldUID != 0)
					{
						UID newFieldUID;
						if (App->GetModule<ModuleScene>()->hasNewUID(fieldUID, newFieldUID))
						{
							optField.value().setter(
								App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(newFieldUID));
						}
						else
						{
							optField.value().setter(
								App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(fieldUID));
						}
					}

					else
					{
						optField.value().setter(nullptr);
					}
				}
				break;
			}

			case FieldType::BOOLEAN:
			{
				std::string valueName = field["name"];
				std::optional<Field<bool>> optField = GetField<bool>(script, valueName);
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
