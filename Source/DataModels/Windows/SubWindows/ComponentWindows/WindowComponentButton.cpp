#include "WindowComponentButton.h"

#include "Components/UI/ComponentButton.h"

#include "Application.h"

WindowComponentButton::WindowComponentButton(ComponentButton* component) : ComponentWindow("Button", component)
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
		float4 colorHovered = asButton->GetColorHovered();
		if (ImGui::ColorEdit4("Color hovered", (float*) &colorHovered))
		{
			asButton->SetColorHovered(colorHovered);
		}

		float4 colorClicked = asButton->GetColorClicked();
		if (ImGui::ColorEdit4("Color clicked", (float*) &colorClicked))
		{
			asButton->SetColorClicked(colorClicked);
		}

		char* sceneName = (char*) asButton->GetSceneName();
		ImGui::InputText("##Scene name", sceneName, 24);
	}
}
