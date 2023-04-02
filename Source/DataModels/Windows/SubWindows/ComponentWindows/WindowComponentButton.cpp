#include "WindowComponentButton.h"

#include "Components/UI/ComponentButton.h"

#include "Application.h"

WindowComponentButton::WindowComponentButton(ComponentButton* component) : ComponentWindow("BUTTON", component)
{
}

WindowComponentButton::~WindowComponentButton()
{
}

void WindowComponentButton::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");

	ComponentButton* asButton = static_cast<ComponentButton*>(component);

	if (asButton)
	{
		ImGui::ColorEdit3("Color Hovered", (float*) &asButton->GetColorHovered());
		ImGui::ColorEdit3("Color Clicked", (float*) &asButton->GetColorClicked());
		char* sceneName = (char*) asButton->GetSceneName();
		ImGui::InputText("##Scene name", sceneName, 24);
	}
}
