#include "WindowComponentAnimation.h"

#include "Windows/EditorWindows/ImporterWindows/WindowStateMachineInput.h"
#include "Components/ComponentAnimation.h"

#include "Application.h"
#include "ModuleEditor.h"


WindowComponentAnimation::WindowComponentAnimation(ComponentAnimation* component) :
	ComponentWindow("ANIMATION", component), inputState(std::make_unique<WindowStateMachineInput>(component))
{
}

WindowComponentAnimation::~WindowComponentAnimation()
{
}

void WindowComponentAnimation::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentAnimation* asAnimation = static_cast<ComponentAnimation*>(component);
	std::shared_ptr<ResourceStateMachine> state = asAnimation->GetStateMachine();
	if (state)
	{
		if (ImGui::Button("Edit StateMachine"))
		{
			App->editor->SetStateMachineWindowEditor(state);
		}
	}
	else
	{
		inputState->DrawWindowContents();
	}
}
