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

			case FieldType::STRING:
			{
				field["name"] = std::get<Field<std::string>>(enumAndValue.second).name.c_str();
				field["value"] = std::get<Field<std::string>>(enumAndValue.second).getter().c_str();
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

			case FieldType::VECTOR:
			{
				Json vectorElements = fields[index];
					
				VectorField vectorField = std::get<VectorField>(enumAndValue.second);
				vectorElements["name"] = vectorField.name.c_str();
				vectorElements["type"] = static_cast<int>(enumAndValue.first);
				Json vectorElementsWithName = vectorElements["vectorElements"];
			
				std::vector<std::any> vectorValue = vectorField.getter();
				
				for (int i = 0; i < vectorValue.size(); ++i) {
				
					switch (vectorField.innerType)
					{
						case FieldType::FLOAT:
							vectorElementsWithName[i]["name"] = std::string(vectorField.name + std::to_string(i)).c_str();
							vectorElementsWithName[i]["value"] = std::any_cast<float>(vectorValue[i]);
							vectorElementsWithName[i]["innerType"] = 0;
							break;

						case FieldType::STRING:
							vectorElementsWithName[i]["name"] = vectorField.name.c_str();
							vectorElementsWithName[i]["value"] = std::any_cast<std::string>(vectorValue[i]).c_str();
							vectorElementsWithName[i]["innerType"] = 1;
							break;

						case FieldType::BOOLEAN:
							vectorElementsWithName[i]["name"] = vectorField.name.c_str();
							vectorElementsWithName[i]["value"] = std::any_cast<bool>(vectorValue[i]);
							vectorElementsWithName[i]["innerType"] = 5;
							break;

						case FieldType::GAMEOBJECT:
							vectorElementsWithName[i]["name"] = vectorField.name.c_str();

							if (std::any_cast<GameObject*>(vectorValue[i]) != nullptr)
							{
								vectorElementsWithName[i]["value"] = std::any_cast<GameObject*>(vectorValue[i])->GetUID();
							}
							else
							{
								vectorElementsWithName[i]["value"] = 0;
							}

							vectorElementsWithName[i]["type"] = static_cast<int>(enumAndValue.first);
							vectorElementsWithName[i]["innerType"] = 2;
							break;

						case FieldType::FLOAT3:
							vectorElementsWithName[i]["name"] = vectorField.name.c_str();
							vectorElementsWithName[i]["value x"] = std::any_cast<float3>(vectorValue[i])[0];
							vectorElementsWithName[i]["value y"] = std::any_cast<float3>(vectorValue[i])[1];
							vectorElementsWithName[i]["value z"] = std::any_cast<float3>(vectorValue[i])[2];
							vectorElementsWithName[i]["innerType"] = 3;
							break;
					}
				}

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
		ENGINE_LOG("%d", static_cast<int>(field["type"]));
		ENGINE_LOG("%d", fieldType);
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

			case FieldType::VECTOR:
			{
				std::string valueName = field["name"];
				//std::optional<Field<VectorField>> vectorField = script->GetField<VectorField>(valueName);
				FieldType innerFieldType = static_cast<FieldType>(static_cast<int>(field["innerType"]));
				/*if (vectorField)
				{*/
					switch (innerFieldType) {
						case FieldType::FLOAT:
							
							ENGINE_LOG("float case");
							
							break;

						case FieldType::STRING:

							ENGINE_LOG("string case");

							break;

						case FieldType::BOOLEAN:
							
							ENGINE_LOG("bool case");

							break;

						case FieldType::GAMEOBJECT:
							
							ENGINE_LOG("GO case");

							break;

						case FieldType::FLOAT3:
							
							ENGINE_LOG("Float3 case");

							break;
					}
				/*	for (int i = 0; i < vectorValue.size(); ++i) {
						ENGINE_LOG("%d", i);
						
					}*/
				//}

				
				break;
			}

			default:
				break;
		}
	}
}
