#include "WindowComponentMockStates.h"

#include "Components/ComponentMockState.h"

WindowComponentMockStates::WindowComponentMockStates(ComponentMockState* component) :
	ComponentWindow("MOCK STATE", component)
{
}

WindowComponentMockStates::~WindowComponentMockStates()
{
}

void WindowComponentMockStates::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); //used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentMockState* asMockState = static_cast<ComponentMockState*>(component);

	if (asMockState)
	{
		ImGui::Text("");

		bool isWinState = asMockState->GetIsWinState();
		bool isFailState = asMockState->GetIsFailState();

		ImGui::Text("Is win state"); ImGui::SameLine();
		if (ImGui::Checkbox("##Is win statr", &isWinState))
		{
			asMockState->SetIsWinState(isWinState);
		}

		ImGui::Text("Is fail state"); ImGui::SameLine();
		if (ImGui::Checkbox("##Is fail statr", &isFailState))
		{
			asMockState->SetIsFailState(isFailState);
		}
	}
}