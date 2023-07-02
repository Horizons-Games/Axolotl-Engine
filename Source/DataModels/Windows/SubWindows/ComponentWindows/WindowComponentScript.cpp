#include "WindowComponentScript.h"

#include "Application.h"
#include "Scene/Scene.h"

#include "Modules/ModuleScene.h"
#include "FileSystem/ModuleFileSystem.h"

#include "Components/ComponentScript.h"

#include "ScriptFactory.h"
#include "IScript.h"
#include "Math/float3.h"

#include "Auxiliar/Reflection/VectorField.h"

WindowComponentScript::WindowComponentScript(ComponentScript* component) :
	ComponentWindow("SCRIPT", component), windowUID(UniqueID::GenerateUID())
{
}


WindowComponentScript::~WindowComponentScript()
{
}

std::string WindowComponentScript::DrawStringField(char* value, const std::string& name)
{
	ImGui::InputText(name.c_str(), value, 24);
	return value;
}

bool WindowComponentScript::DrawBoolField(bool value, const std::string& name)
{
	ImGui::Checkbox(name.c_str(), &value);
	return value;
}

float WindowComponentScript::DrawFloatField(float value, const std::string& name)
{
	ImGui::DragFloat(name.c_str(), &value, 0.05f, -50.0f, 50.0f, "%.2f");
	return value;
}

math::float3 WindowComponentScript::DrawFloat3Field(math::float3 value, const std::string& name)
{
	ImGui::DragFloat3(name.c_str(), (&value[2], &value[1], &value[0]), 0.05f, -50.0f, 50.0f, "%.2f");
	return value;
}

GameObject* WindowComponentScript::DrawGOField(GameObject* value, const std::string& name) 
{
	std::string gameObjectSlot = "Drag a GameObject here";
	if (value != nullptr)
	{
		gameObjectSlot = value->GetName().c_str();
	}

	ImGui::Button((gameObjectSlot + "##" + std::to_string(windowUID)).c_str(), ImVec2(208.0f, 20.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
		{
			UID draggedGameObjectID = *(UID*)payload->Data;
			GameObject* draggedGameObject =
				App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);

			if (draggedGameObject)
			{
				ImGui::EndDragDropTarget();
				return draggedGameObject; 
			}
		}

		ImGui::EndDragDropTarget();
	}
	
	ImGui::SameLine(0.0f, 3.0f);
	ImGui::Text(name.c_str());
	ImGui::SameLine();
	if (ImGui::Button(("Remove"+name+"##"+ std::to_string(windowUID)).c_str()))
	{
		return nullptr;
	}

	return value;
}

void WindowComponentScript::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text("");

	std::vector<const char*> constructors = App->GetScriptFactory()->GetConstructors();
	ComponentScript* script = static_cast<ComponentScript*>(component);

	if (!script)
	{
		return;
	}

	std::string label = "Select Script";
	std::string separator = "##";
	std::string thisID = std::to_string(windowUID);
	std::string finalLabel = label + separator + thisID;

	const IScript* scriptObject = script->GetScript();

	if (!scriptObject)
	{
		if (ImGui::ListBox(finalLabel.c_str(), &current_item, constructors.data(), static_cast<int>(constructors.size()), 5))
		{
			ChangeScript(script, constructors[current_item]);
			ENGINE_LOG("%s SELECTED, drawing its contents.", script->GetConstructName().c_str());
		}

		label = "Create Script##";
		finalLabel = label + thisID;
		if (ImGui::Button(finalLabel.c_str()))
		{
			ImGui::OpenPopup("Create new script");
		}

		ImGui::SetNextWindowSize(ImVec2(280, 75));

		if (ImGui::BeginPopupModal("Create new script", nullptr,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
		{
			OpenCreateNewScriptPopUp();
			ImGui::EndPopup();
		}
		return;
	}

	ImGui::Separator();

	std::string scriptName = script->GetConstructName().c_str();
	std::string scriptExtension = ".cpp:";
	std::string fullScriptName = scriptName + scriptExtension;
	ImGui::Text(fullScriptName.c_str());

	if (ImGui::GetWindowWidth() > static_cast<float>(fullScriptName.size()) * 13.0f)
	{
		ImGui::SameLine(ImGui::GetWindowWidth() - 110.0f);
	}

	else
	{
		ImGui::SameLine();
	}

	label = "Remove Script##";
	finalLabel = label + thisID;
	if (ImGui::Button(finalLabel.c_str()))
	{
		ENGINE_LOG("%s REMOVED, showing list of available scripts.", script->GetConstructName().c_str());

		script->SetScript(nullptr); // This deletes the script itself
		script->SetConstuctor("");	// And this makes that it is also deleted from the serialization
	}

	for (TypeFieldPair enumAndMember : scriptObject->GetFields())
	{
		ValidFieldType member = enumAndMember.second;
		
		switch (enumAndMember.first)
		{
			case FieldType::FLOAT:
			{
				Field<float> floatField = std::get<Field<float>>(member);
				float value = floatField.getter();

				label = floatField.name;
				finalLabel = label + separator + thisID;

				floatField.setter(DrawFloatField(value, finalLabel.c_str()));
				break;
			}

			case FieldType::FLOAT3:
			{
				Field<float3> float3Field = std::get<Field<float3>>(member);
				float3 value = float3Field.getter();
				
				float3Field.setter(DrawFloat3Field(value, float3Field.name.c_str()));
				break;
			}

			case FieldType::STRING:
			{
				Field<std::string> stringField = std::get<Field<std::string>>(member);
				std::string value = stringField.getter();
			
				label = stringField.name;
				finalLabel = label + separator + thisID;

				stringField.setter(DrawStringField(value.data(), finalLabel).c_str());
				
				break;
			}

			case FieldType::BOOLEAN:
			{
				Field<bool> booleanField = std::get<Field<bool>>(member);
				bool value = booleanField.getter();

				label = booleanField.name;
				finalLabel = label + separator + thisID;
				
				booleanField.setter(DrawBoolField(value, finalLabel.c_str()));
				
				break;
			}

			case FieldType::GAMEOBJECT:
			{
				Field<GameObject*> gameObjectField = std::get<Field<GameObject*>>(member);
				GameObject* value = gameObjectField.getter();


				GameObject* draggedObject = DrawGOField(value, gameObjectField.name);

				gameObjectField.setter(draggedObject);

				break;
			}

			case FieldType::VECTOR:
			{
				VectorField vectorField = std::get<VectorField>(member);

				std::function<std::any(const std::any&, const std::string&)> elementDrawer =
					[this, &vectorField](const std::any& value, const std::string& name) -> std::any
				{
					switch (vectorField.innerType)
					{
					case FieldType::FLOAT:
						return float(DrawFloatField(std::any_cast<float>(value), name));
					case FieldType::STRING:
						return std::string(DrawStringField(std::any_cast<std::string>(value).data(), name).c_str());
					case FieldType::BOOLEAN:
						return bool(DrawBoolField(std::any_cast<bool>(value), name));
					case FieldType::GAMEOBJECT:
						return std::any(DrawGOField(std::any_cast<GameObject*>(value), name));
					case FieldType::FLOAT3:
						return float3(DrawFloat3Field(std::any_cast<float3>(value), name));
					}
					return std::any();  // Default return 
				};

				std::vector<std::any> vectorValue = vectorField.getter();

				for (int i = 0; i < vectorValue.size(); ++i)
				{
					vectorValue[i] = elementDrawer(vectorValue[i], vectorField.name + std::to_string(i));
				}

				vectorField.setter(vectorValue);

				break;
			}

			default:
				break;
		}

	}
}

void WindowComponentScript::ChangeScript(ComponentScript* newScript, const char* selectedScript)
{
	newScript->SetConstuctor(selectedScript);
	IScript* Iscript = App->GetScriptFactory()->ConstructScript(selectedScript);
	Iscript->SetGameObject(component->GetOwner());
	Iscript->SetApplication(App.get());
	newScript->SetScript(Iscript);
}

void WindowComponentScript::OpenCreateNewScriptPopUp()
{
	static char name[FILENAME_MAX] = "NewScript";
	ImGui::InputText("Script Name", name, IM_ARRAYSIZE(name));
	ImGui::NewLine();

	ImGui::SameLine(ImGui::GetWindowWidth() - 120);
	if (ImGui::Button("Save", ImVec2(50, 20)))
	{
		AddNewScriptToProject(name);
		// reset the input string contents
		strcpy_s(name, "NewScript");
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 60);
	if (ImGui::Button("Cancel"))
	{
		ImGui::CloseCurrentPopup();
	}
}

void WindowComponentScript::AddNewScriptToProject(const std::string& scriptName)
{
	std::string scriptsPath = "Scripts/";

	std::string scriptHeaderPath = scriptsPath + scriptName + ".h";
	std::string scriptSourcePath = scriptsPath + scriptName + ".cpp";

	const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	// Both header and source have the same name, so only checking the header is enough
	if (fileSystem->Exists(scriptHeaderPath.c_str()))
	{
		ENGINE_LOG("That name is already in use, please use a different one");
		return;
	}

	char* headerBuffer = nullptr;
	char* sourceBuffer = nullptr;

	unsigned int headerBufferSize = fileSystem->Load("Source/PreMades/TemplateHeaderScript", headerBuffer);
	unsigned int sourceBufferSize = fileSystem->Load("Source/PreMades/TemplateSourceScript", sourceBuffer);

	std::string headerString(headerBuffer, headerBufferSize);
	delete headerBuffer;
	std::string sourceString(sourceBuffer, sourceBufferSize);
	delete sourceBuffer;

	ReplaceSubstringsInString(headerString, "{0}", scriptName);
	ReplaceSubstringsInString(sourceString, "{0}", scriptName);

	fileSystem->Save(scriptHeaderPath.c_str(), headerString.c_str(), headerString.size());
	fileSystem->Save(scriptSourcePath.c_str(), sourceString.c_str(), sourceString.size());

	ENGINE_LOG("New script %s created", scriptName.c_str());
}

void WindowComponentScript::ReplaceSubstringsInString(std::string& stringToReplace,
	const std::string& from, const std::string& to)
{
	if (from.empty())
	{
		return;
	}

	size_t startPos = 0;
	while ((startPos = stringToReplace.find(from, startPos)) != std::string::npos)
	{
		stringToReplace.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}