#include "StdAfx.h"

#include "WindowComponentAnimation.h"

#include "Components/ComponentAnimation.h"
#include "Windows/EditorWindows/ImporterWindows/WindowStateMachineInput.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "Resources/ResourceStateMachine.h"

WindowComponentAnimation::WindowComponentAnimation(ComponentAnimation* component) :
	ComponentWindow("ANIMATION", component),
	inputState(std::make_unique<WindowStateMachineInput>(component))
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
	if (asAnimation)
	{
		bool drawBones = asAnimation->IsDrawBonesActivated();
		ImGui::Text("Draw Bones:");
		ImGui::SameLine();
		if (ImGui::Checkbox("##DrawBones", &drawBones))
		{
			asAnimation->ActivateDrawBones(drawBones);
		}

		std::shared_ptr<ResourceStateMachine> state = asAnimation->GetStateMachine();
		if (state)
		{
			ImGui::Text(state->GetFileName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Edit StateMachine"))
			{
				App->GetModule<ModuleEditor>()->SetStateMachineWindowEditor(state);
			}
			ImGui::SameLine();
			if (ImGui::Button("x"))
			{
				asAnimation->SetStateMachine(nullptr);
			}
		}
		else
		{
			inputState->DrawWindowContents();
		}
	}
}
