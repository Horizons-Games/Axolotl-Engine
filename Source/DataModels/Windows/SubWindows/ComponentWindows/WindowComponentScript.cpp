#include "WindowComponentScript.h"
#include "Components/ComponentScript.h"
#include "Application.h"
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
	if (ImGui::ListBox("Constructor", &current_item, &firstElementAsCharPtr, (int)(constructors.size()), 3))
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
		for (TypeFieldPair enumAndMember : scriptObject->GetFields())
		{
			ValidFieldType member = enumAndMember.second;
			switch (enumAndMember.first)
			{
				case FieldType::FLOAT:
				{
					Field<float> floatField = std::get<Field<float>>(member);
					float value = floatField.getter();
					if (ImGui::SliderFloat(floatField.name.c_str(), &value, 0, 50))
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

				default:
					break;
			}
		}
	}
}