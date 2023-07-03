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
			vectorElements["innerType"] = static_cast<int>(vectorField.innerType);

			std::vector<std::any> vectorValue = vectorField.getter();

			for (int i = 0; i < vectorValue.size(); ++i) {

				switch (vectorField.innerType)
				{
				case FieldType::FLOAT:
					vectorElementsWithName[i]["name"] = std::string(vectorField.name + std::to_string(i)).c_str();
					vectorElementsWithName[i]["value"] = std::any_cast<float>(vectorValue[i]);
					//vectorElements["innerType"] = static_cast<int>(vectorField.innerType);
					break;

				case FieldType::STRING:
					vectorElementsWithName[i]["name"] = vectorField.name.c_str();
					vectorElementsWithName[i]["value"] = std::any_cast<std::string>(vectorValue[i]).c_str();
					//vectorElements["innerType"] = static_cast<int>(vectorField.innerType);
					break;

				case FieldType::BOOLEAN:
					vectorElementsWithName[i]["name"] = vectorField.name.c_str();
					vectorElementsWithName[i]["value"] = std::any_cast<bool>(vectorValue[i]);
					//vectorElements["innerType"] = static_cast<int>(vectorField.innerType);
					break;

				case FieldType::GAMEOBJECT:
					vectorElementsWithName[i]["name"] = vectorField.name.c_str();

					if (std::any_cast<GameObject*>(vectorValue[i]) != nullptr)
					{
						vectorElementsWithName[i]["value"] = std::any_cast<GameObject*>(vectorValue[i])->GetUID();
					}
					else
					{
						vectorElementsWithName[i]["value"] = static_cast<int>(vectorField.innerType);
					}

					vectorElementsWithName[i]["type"] = static_cast<int>(enumAndValue.first);
					//vectorElements["innerType"] = static_cast<int>(vectorField.innerType);
					break;

				case FieldType::FLOAT3:
					vectorElementsWithName[i]["name"] = vectorField.name.c_str();
					vectorElementsWithName[i]["value x"] = std::any_cast<float3>(vectorValue[i])[0];
					vectorElementsWithName[i]["value y"] = std::any_cast<float3>(vectorValue[i])[1];
					vectorElementsWithName[i]["value z"] = std::any_cast<float3>(vectorValue[i])[2];
					//vectorElements["innerType"] = static_cast<int>(vectorField.innerType);
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
	/*Json vectorElements = fields;
	Json vectorElementsWithName = vectorElements["vectorElements"];*/

	for (unsigned int i = 0; i < fields.Size(); ++i)
	{
		Json field = fields[i];
		FieldType fieldType = static_cast<FieldType>(static_cast<int>(field["type"]));
		//ENGINE_LOG("%d", static_cast<int>(field["type"]));
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
			Json vectorElements = field["vectorElements"];
			ENGINE_LOG("%d", vectorElements.Size());
			std::optional<Field<std::vector<std::any>>> vectorField = script->GetField<std::vector<std::any>>(valueName);
			if (!vectorField)
			{
				continue;
			}
			std::vector<std::any> vectorCase = vectorField.value().getter();

			for (unsigned int j = 0; j < vectorElements.Size(); ++j)
			{

				FieldType innerFieldType = static_cast<FieldType>(static_cast<int>(field["innerType"]));

				switch (innerFieldType) 
				{
				case FieldType::FLOAT:

					vectorCase[j] = (float)vectorElements[j]["value"];

					break;

				case FieldType::STRING:

					vectorCase[j] = (std::string)vectorElements[j]["value"];

					break;

				case FieldType::BOOLEAN:

					vectorCase[j] = (bool)vectorElements[j]["value"];

					break;

				case FieldType::GAMEOBJECT:
				{
					UID fieldUID = (UID)vectorElements[j]["value"];
					if (fieldUID != 0)
					{
						UID newFieldUID;
						if (App->GetModule<ModuleScene>()->hasNewUID(fieldUID, newFieldUID))
						{
							vectorCase[j] = (GameObject*)App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(newFieldUID);
						}
						else
						{
							vectorCase[j] = (GameObject*)App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(fieldUID);
						}
					}
					else
					{
						vectorCase[j] = (GameObject*)nullptr;
					}

					break;
				}
				case FieldType::FLOAT3:
					float3 vec3(vectorElements[j]["value x"], vectorElements[j]["value y"], vectorElements[j]["value z"]);

					vectorCase[j] = vec3;
					break;
				}
			}
			vectorField.value().setter(vectorCase);
			break;
		}

		default:
			break;
		}
	}
}
