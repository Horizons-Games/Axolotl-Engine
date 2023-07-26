#include "StdAfx.h"

#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "Scripting/Script.h"
#include "Scripting/ScriptFactory.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "ComponentRigidBody.h"

namespace
{
// helper method to handle exception and validity state of the component
// this logic is the same for all the method of component script, so we can just encapsulate it here
template<typename Fun>
void RunScriptMethodAndHandleException(bool& scriptFailedState, ComponentScript* script, Fun&& scriptMethod)
{
	try
	{
		if (scriptFailedState)
		{
			return;
		}
		scriptMethod();
	}
	catch (const ComponentNotFoundException& exception)
	{
		LOG_ERROR("Error during execution of script {}, owned by {}. Error message: {}",
				  script->GetConstructName(),
				  script->GetOwner(),
				  exception.what());
		scriptFailedState = true;
	}
}
} // namespace

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
	failed = false;
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  if (!initialized && GetOwner()->IsActive() && ScriptCanBeCalled())
										  {
											  script->Init();
											  initialized = true;
										  }
									  });
}

void ComponentScript::Start()
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  if (!started && IsEnabled() && ScriptCanBeCalled())
										  {
											  script->Start();
											  started = true;
										  }
									  });
}

void ComponentScript::PreUpdate()
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  if (IsEnabled() && ScriptCanBeCalled())
										  {
											  script->PreUpdate(App->GetDeltaTime());
										  }
									  });
}

void ComponentScript::Update()
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  if (IsEnabled() && ScriptCanBeCalled())
										  {
											  script->Update(App->GetDeltaTime());
										  }
									  });
}

void ComponentScript::PostUpdate()
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  if (IsEnabled() && ScriptCanBeCalled())
										  {
											  script->PostUpdate(App->GetDeltaTime());
										  }
									  });
}

void ComponentScript::OnCollisionEnter(ComponentRigidBody* other)
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this, &other]
									  {
										  if (IsEnabled() && ScriptCanBeCalled())
										  {
											  script->OnCollisionEnter(other);
										  }
									  });
}
void ComponentScript::OnCollisionExit(ComponentRigidBody* other)
{
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this, &other]
									  {
										  if (IsEnabled() && ScriptCanBeCalled())
										  {
											  script->OnCollisionExit(other);
										  }
									  });
}

void ComponentScript::CleanUp()
{
	// reset the failed state again, so even if the script had an error during its execution, try to clean up
	failed = false;
	RunScriptMethodAndHandleException(failed,
									  this,
									  [this]
									  {
										  // Call CleanUp regardless if the script is active or not
										  if (script)
										  {
											  script->CleanUp();
										  }
										  started = false;
										  initialized = false;
									  });
	// reset the failed state again in case there was an error during cleanup
	failed = true;
}

bool ComponentScript::ScriptCanBeCalled() const
{
	return script && App->IsOnPlayMode() && !App->GetScriptFactory()->IsCompiling();
}

void ComponentScript::InternalSave(Json& meta)
{
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

				for (int i = 0; i < vectorValue.size(); ++i)
				{
					switch (vectorField.innerType)
					{
						case FieldType::FLOAT:
							vectorElementsWithName[i]["value"] = std::any_cast<float>(vectorValue[i]);
							break;

						case FieldType::STRING:
							vectorElementsWithName[i]["value"] = std::any_cast<std::string>(vectorValue[i]).c_str();
							break;

						case FieldType::BOOLEAN:
							vectorElementsWithName[i]["value"] = std::any_cast<bool>(vectorValue[i]);
							break;

						case FieldType::GAMEOBJECT:

							if (std::any_cast<GameObject*>(vectorValue[i]) != nullptr)
							{
								vectorElementsWithName[i]["value"] =
									std::any_cast<GameObject*>(vectorValue[i])->GetUID();
							}
							else
							{
								vectorElementsWithName[i]["value"] = 0;
							}

							break;

						case FieldType::FLOAT3:
							vectorElementsWithName[i]["value x"] = std::any_cast<float3>(vectorValue[i])[0];
							vectorElementsWithName[i]["value y"] = std::any_cast<float3>(vectorValue[i])[1];
							vectorElementsWithName[i]["value z"] = std::any_cast<float3>(vectorValue[i])[2];
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

void ComponentScript::InternalLoad(const Json& meta)
{
	constructName = meta["constructName"];
	script = App->GetScriptFactory()->ConstructScript(constructName.c_str());

	if (script == nullptr)
	{
		LOG_WARNING("Script {} unable to be constructed", constructName);
		return;
	}

	script->SetApplication(App.get());
	script->SetOwner(GetOwner());
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
				std::optional<Field<std::vector<std::any>>> vectorField =
					script->GetField<std::vector<std::any>>(valueName);
				if (!vectorField)
				{
					continue;
				}
				std::vector<std::any> vectorCase;

				for (unsigned int j = 0; j < vectorElements.Size(); ++j)
				{
					FieldType innerFieldType = static_cast<FieldType>(static_cast<int>(field["innerType"]));

					switch (innerFieldType)
					{
						case FieldType::FLOAT:

							vectorCase.push_back((float) vectorElements[j]["value"]);

							break;

						case FieldType::STRING:

							vectorCase.push_back((std::string) vectorElements[j]["value"]);

							break;

						case FieldType::BOOLEAN:

							vectorCase.push_back((bool) vectorElements[j]["value"]);

							break;

						case FieldType::GAMEOBJECT:
						{
							UID fieldUID = (UID) vectorElements[j]["value"];
							if (fieldUID != 0)
							{
								UID newFieldUID;
								if (App->GetModule<ModuleScene>()->hasNewUID(fieldUID, newFieldUID))
								{
									vectorCase.push_back((GameObject*) App->GetModule<ModuleScene>()
															 ->GetLoadedScene()
															 ->SearchGameObjectByID(newFieldUID));
								}
								else
								{
									vectorCase.push_back((GameObject*) App->GetModule<ModuleScene>()
															 ->GetLoadedScene()
															 ->SearchGameObjectByID(fieldUID));
								}
							}
							else
							{
								vectorCase.push_back((GameObject*) nullptr);
							}

							break;
						}
						case FieldType::FLOAT3:
							vectorCase.push_back(float3(vectorElements[j]["value x"],
														vectorElements[j]["value y"],
														vectorElements[j]["value z"]));
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

void ComponentScript::SignalEnable()
{
	if (App->IsOnPlayMode() && !App->GetModule<ModuleScene>()->IsLoading())
	{
		Init();
		Start();
	}
}
