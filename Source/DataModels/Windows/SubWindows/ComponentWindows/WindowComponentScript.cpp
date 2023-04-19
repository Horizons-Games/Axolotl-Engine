#include "WindowComponentScript.h"
#include "Components/ComponentScript.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Modules/ModuleScene.h"
#include "ScriptFactory.h"
#include "IScript.h"

WindowComponentScript::WindowComponentScript(ComponentScript* component) :
	ComponentWindow("SCRIPT", component)
{
}


WindowComponentScript::~WindowComponentScript()
{
}

void WindowComponentScript::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text("");

	std::vector<std::string> constructors = App->scriptFactory->GetConstructors();
	ComponentScript* script = static_cast<ComponentScript*>(this->component);
	const char* firstElementAsCharPtr = constructors.data()->c_str();
	if (ImGui::ListBox("Available Scripts", &current_item, &firstElementAsCharPtr, (int)(constructors.size()), 3))
	{
		script->SetConstuctor(constructors[current_item]);
		IScript* Iscript = App->scriptFactory->ConstructScript(constructors[current_item]);
		Iscript->SetGameObject(component->GetOwner());
		Iscript->SetApplication(App.get());
		script->SetScript(Iscript);
		ENGINE_LOG("SCRIPT SELECTED, DRAWING ITS CONTENTS");
	}

	IScript* scriptObject = script->GetScript();
	if (scriptObject)
	{
		ImGui::Separator();

		std::string scriptName = script->GetConstructName().c_str();
		std::string scriptExtension = ".cpp:";
		std::string fullScriptName = scriptName + scriptExtension;
		ImGui::Text(fullScriptName.c_str());

		for (TypeFieldPair enumAndMember : scriptObject->GetFields())
		{
			ValidFieldType member = enumAndMember.second;
			switch (enumAndMember.first)
			{
				case FieldType::FLOAT:
				{
					Field<float> floatField = std::get<Field<float>>(member);
					float value = floatField.getter();
					if (ImGui::DragFloat(floatField.name.c_str(), &value, 0.05f, -50.0f, 50.0f, "%.2f"))
					{
						floatField.setter(value);
					}
					break;
				}

				case FieldType::STRING:
				{
					Field<std::string> stringField = std::get<Field<std::string>>(member);
					std::string value = stringField.getter();
				
					if (ImGui::InputText(stringField.name.c_str(), (char*)(value.c_str()), 24))
					{
						stringField.setter(value);
					}
					break;
				}

				case FieldType::GAMEOBJECT:
				{
					Field<GameObject*> gameObjectField = std::get<Field<GameObject*>>(member);
					GameObject* value = gameObjectField.getter();

					std::string gameObjectName = "Empty";
					if (value != nullptr)
					{
						gameObjectName = value->GetName().c_str();
					}

					ImGui::Button(gameObjectName.c_str(), ImVec2(208.0f, 20.0f));

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
						{
							UID draggedGameObjectID = *(UID*)payload->Data;
							GameObject* draggedGameObject =
								App->scene->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);

							if (draggedGameObject)
							{
								gameObjectField.setter(draggedGameObject);
							}
						}

						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine(0.0f, 3.0f);
					ImGui::Text(gameObjectField.name.c_str());

					break;
				}

				default:
					break;
			}
		}
	}
}