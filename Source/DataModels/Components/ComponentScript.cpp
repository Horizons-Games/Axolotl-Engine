#include "StdAfx.h"

#include "ComponentScript.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "FileSystem/ModuleResources.h"
#include "Animation/StateMachine.h"

#include "Scripting/Script.h"
#include "Scripting/ScriptFactory.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "ComponentRigidBody.h"

#include "Exceptions/ComponentNotFoundException.h"
#include "Exceptions/ScriptAssertFailedException.h"

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
	catch (const ScriptAssertFailedException& exception)
	{
		LOG_ERROR("Assertion failed during execution of script {}, owned by {}. Error message: {}",
				  script->GetConstructName(),
				  script->GetOwner(),
				  exception.what());
		scriptFailedState = true;
	}
}

template<typename T>
std::optional<Field<T>> GetField(IScript* script, const std::string& name)
{
	for (const TypeFieldPair& enumAndType : script->GetFields())
	{
		if (TypeToEnum<T>::value == enumAndType.first)
		{
			const Field<T>& field = std::get<Field<T>>(enumAndType.second);
			if (field.name == name)
			{
				return field;
			}
		}
	}
	return std::nullopt;
}

template<>
inline std::optional<Field<std::vector<std::any>>> GetField(IScript* script, const std::string& name)
{
	for (const TypeFieldPair& enumAndType : script->GetFields())
	{
		if (FieldType::VECTOR == enumAndType.first)
		{
			const VectorField& field = std::get<VectorField>(enumAndType.second);
			if (field.name == name)
			{
				return field;
			}
		}
	}
	return std::nullopt;
}

} // namespace

ComponentScript::ComponentScript(bool active, GameObject* owner) :
	Component(ComponentType::SCRIPT, active, owner, true),
	script(nullptr)
{
}

ComponentScript::ComponentScript(const ComponentScript& other) :
	Component(other),
	constructName(other.constructName),
	initialized(other.initialized),
	started(other.started),
	script(App->GetScriptFactory()->ConstructScript(other.constructName.c_str()))
{
	if (script == nullptr)
	{
		return;
	}
	for (const TypeFieldPair& typeFieldPair : script->GetFields())
	{
		switch (typeFieldPair.first)
		{
			case FieldType::FLOAT:
			{
				const Field<float>& field = std::get<Field<float>>(typeFieldPair.second);
				std::optional<Field<float>> optField = GetField<float>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			case FieldType::STRING:
			{
				const Field<std::string>& field = std::get<Field<std::string>>(typeFieldPair.second);
				std::optional<Field<std::string>> optField = GetField<std::string>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			case FieldType::BOOLEAN:
			{
				const Field<bool>& field = std::get<Field<bool>>(typeFieldPair.second);
				std::optional<Field<bool>> optField = GetField<bool>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			case FieldType::FLOAT3:
			{
				const Field<float3>& field = std::get<Field<float3>>(typeFieldPair.second);
				std::optional<Field<float3>> optField = GetField<float3>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			case FieldType::GAMEOBJECT:
			{
				const Field<GameObject*>& field = std::get<Field<GameObject*>>(typeFieldPair.second);
				std::optional<Field<GameObject*>> optField = GetField<GameObject*>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			case FieldType::VECTOR:
			{
				const VectorField& field = std::get<VectorField>(typeFieldPair.second);
				std::optional<Field<std::vector<std::any>>> optField =
					GetField<std::vector<std::any>>(other.GetScript(), field.name);
				if (optField.has_value())
				{
					field.setter(optField->getter());
				}
				break;
			}
			default:
				break;
		}
	}
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

void ComponentScript::SetOwner(GameObject* owner)
{
	Component::SetOwner(owner);
	if (script)
	{
		script->SetOwner(owner);
	}
}

void ComponentScript::SetScript(IScript* script)
{
	this->script = script;
	if (script)
	{
		script->SetContainer(this);
	}
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
	for (const TypeFieldPair& enumAndValue : script->GetFields())
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
				const Field<float3>& fieldInstance = std::get<Field<float3>>(enumAndValue.second);
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

				GameObject* fieldValue = std::get<Field<GameObject*>>(enumAndValue.second).getter();
				if (fieldValue != nullptr)
				{
					field["value"] = fieldValue->GetUID();
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

				const VectorField& vectorField = std::get<VectorField>(enumAndValue.second);
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

			case FieldType::STATEMACHINE:
			{
				field["name"] = std::get<Field<StateMachine*>>(enumAndValue.second).name.c_str();

				std::shared_ptr<ResourceStateMachine> resource =
					std::get<Field<StateMachine*>>(enumAndValue.second).getter()->GetStateMachine();
				if (resource)
				{
					field["valueUID"] = resource->GetUID();
					field["valuePath"] = resource->GetAssetsPath().c_str();
				}
				else
				{
					field["valueUID"] = 0;
					field["valuePath"] = "";
				}

				field["type"] = static_cast<int>(enumAndValue.first);
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
	// Don't call InstantiateScript from here, since that's not the expected behaviour
	// For more detail, see comment in InstantiateScript
	if (script == nullptr)
	{
		LOG_ERROR("Trying to load ComponentScript owned by {} without a Script instance; expected one of type {}. Did "
				  "you forget to call InstantiateScript?",
				  GetOwner(),
				  static_cast<std::string>(meta["constructName"]));
		return;
	}

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
						if (App->GetModule<ModuleScene>()->HasNewUID(fieldUID, newFieldUID))
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
				std::optional<Field<float3>> optField = GetField<float3>(script, valueName);
				if (optField)
				{
					float3 vec3(field["value x"], field["value y"], field["value z"]);
					optField.value().setter(vec3);
				}
				break;
			}

			case FieldType::STATEMACHINE:
			{
				std::string valueName = field["name"];
				std::optional<Field<StateMachine*>> optField = GetField<StateMachine*>(script, valueName);
				if (optField)
				{
					std::shared_ptr<ResourceStateMachine> resourceState;
#ifdef ENGINE
					std::string path = field["valuePath"];
					bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
					if (resourceExists)
					{
						resourceState = App->GetModule<ModuleResources>()->RequestResource<ResourceStateMachine>(path);
					}
#else
					UID uidState = field["valueUID"];
					resourceState = App->GetModule<ModuleResources>()->SearchResource<ResourceStateMachine>(uidState);

#endif
					if (resourceState)
					{
						optField.value().getter()->SetStateMachine(resourceState);
					}
				}
				break;
			}

			case FieldType::VECTOR:
			{
				std::string valueName = field["name"];
				Json vectorElements = field["vectorElements"];
				std::optional<Field<std::vector<std::any>>> vectorField =
					GetField<std::vector<std::any>>(script, valueName);
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
								if (App->GetModule<ModuleScene>()->HasNewUID(fieldUID, newFieldUID))
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

void ComponentScript::InstantiateScript(const Json& jsonComponent)
{
	constructName = jsonComponent["constructName"];
	script = App->GetScriptFactory()->ConstructScript(constructName.c_str());

	if (script == nullptr)
	{
		LOG_ERROR("Script {} unable to be constructed", constructName);
		return;
	}

	script->SetOwner(GetOwner());
	script->SetContainer(this);
}
