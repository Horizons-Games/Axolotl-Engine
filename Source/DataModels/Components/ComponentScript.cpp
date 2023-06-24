#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "Scripting/Script.h"
#include "Scripting/ScriptFactory.h"

#include "FileSystem/Json.h"

#include "Scene/Scene.h"
#include "Modules/ModuleScene.h"
#include "Math/float3.h"

ComponentScript::ComponentScript(bool active, GameObject* owner) : 
	Component(ComponentType::SCRIPT, active, owner, true), script(nullptr)
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

			case FieldType::FLOAT3:
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

			case FieldType::VECTOR:
			{
				VectorField vectorField = std::get<VectorField>(enumAndValue.second);
				std::function<std::any(const std::any&, const std::string&)> elementSaver =
					[this, &vectorField](const std::any& value, const std::string& name) -> std::any
				{
					switch (vectorField.innerType)
					{
					case FieldType::FLOAT:
						return float();
					case FieldType::STRING:
						return std::string();
					case FieldType::BOOLEAN:
						return bool();
					case FieldType::GAMEOBJECT:
						return std::any();
					case FieldType::FLOAT3:
						return float3();
					break;
					}
					return std::any();  // Default return 
				};

				std::vector<std::any> vectorValue = vectorField.getter();

				for (int i = 0; i < vectorValue.size(); ++i)
				{
					vectorValue[i] = elementSaver(vectorValue[i], vectorField.name + std::to_string(i));
				}
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
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	constructName = meta["constructName"];
	script = App->GetScriptFactory()->ConstructScript(constructName.c_str());

	if (script == nullptr)
	{
		return;
	}

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
			case FieldType::FLOAT3:
			{
				std::string valueName = field["name"];
				std::optional<Field<float3>> optField = script->GetField<float3>(valueName);
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
				std::optional<Field<std::string>> optField = script->GetField<std::string>(valueName);
				if (optField)
				{
					optField.value().setter(field["value"]);
				}
				break;
			}

			case FieldType::GAMEOBJECT:
			{
				std::string valueName = field["name"];
				std::optional<Field<GameObject*>> optField = script->GetField<GameObject*>(valueName);
				if (optField)
				{
					UID fieldUID = field["value"];
					if (fieldUID != 0)
					{
						UID newFieldUID;
						if (App->GetModule<ModuleScene>()->hasNewUID(fieldUID, newFieldUID))
						{
							optField.value().setter(App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(newFieldUID));
						}
						else 
						{
							optField.value().setter(App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(fieldUID));
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
				std::optional<Field<bool>> optField = script->GetField<bool>(valueName);
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
