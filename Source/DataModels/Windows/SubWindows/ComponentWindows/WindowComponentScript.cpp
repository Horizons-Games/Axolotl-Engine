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

	std::vector<const char*> constructors = App->scriptFactory->GetConstructors();
	ComponentScript* script = static_cast<ComponentScript*>(component);

	if (ImGui::ListBox("Constructor", &current_item, constructors.data(), (int)(constructors.size()), 3))
	{
		if (script->GetConstructName() != constructors[current_item])
		{
			ChangeScript(script, constructors[current_item]);
			ENGINE_LOG("%s SELECTED, drawing its contents.", script->GetConstructName().c_str());
		}
	}

	IScript* scriptObject = script->GetScript();
	if (scriptObject)
	{
		ImGui::Separator();

		std::string scriptName = script->GetConstructName().c_str();
		std::string scriptExtension = ".cpp:";
		std::string fullScriptName = scriptName + scriptExtension;
		ImGui::Text(fullScriptName.c_str());

		if (ImGui::GetWindowWidth() > (float)fullScriptName.size() * 13.0f)
		{
			ImGui::SameLine(ImGui::GetWindowWidth() - 100.0f);
		}
		else
		{
			ImGui::SameLine();
		}

		if (ImGui::Button("Reset Script"))
		{
			ComponentScript* newScript = static_cast<ComponentScript*>(component);

			ChangeScript(newScript, newScript->GetConstructName().c_str());
			ENGINE_LOG("%s RESET, drawing its contents again.", newScript->GetConstructName().c_str());
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

					std::string gameObjectName = "Drag a GameObject here";
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
					ImGui::SameLine();
					if (ImGui::Button("Remove GO"))
					{
						gameObjectField.setter(nullptr);
					}

					break;
				}

				case FieldType::BOOLEAN:
				{
					Field<bool> stringField = std::get<Field<bool>>(member);
					bool value = stringField.getter();

					if (ImGui::Checkbox(stringField.name.c_str(), &value))
					{
						stringField.setter(value);
					}
					break;
				}

				default:
					break;
			}
		}
	}
}

void WindowComponentScript::ChangeScript(ComponentScript* newScript, const char* selectedScript)
{
	newScript->SetConstuctor(selectedScript);
	IScript* Iscript = App->scriptFactory->ConstructScript(selectedScript);
	Iscript->SetGameObject(component->GetOwner());
	Iscript->SetApplication(App.get());
	newScript->SetScript(Iscript);
}