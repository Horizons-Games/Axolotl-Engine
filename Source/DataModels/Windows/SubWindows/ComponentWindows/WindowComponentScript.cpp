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

void WindowComponentScript::DrawWindowContents() {
	std::vector<const char*> constructors = App->scriptFactory->GetConstructors();
	if (ImGui::ListBox("Constructor", &current_item, constructors.data(), constructors.size(), 3)) {
		ComponentScript* script = static_cast<ComponentScript*>(this->component);
		script->SetConstuctor(constructors[current_item]);
		IScript* Iscript = App->scriptFactory->GetScript(constructors[current_item]);
		Iscript->SetGameObject(component->GetOwner());
		Iscript->SetApplication(App.get());
		script->SetScript(Iscript);
		ENGINE_LOG("DRAW WINDOWS CONTENTS");
	}
}