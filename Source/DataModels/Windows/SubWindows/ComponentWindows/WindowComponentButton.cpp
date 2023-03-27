#include "WindowComponentButton.h"

#include "Components/UI/ComponentButton.h"

#include "Application.h"


WindowComponentButton::WindowComponentButton(ComponentButton* component) :
	ComponentWindow("BUTTON", component)
{
}

WindowComponentButton::~WindowComponentButton()
{
}

void WindowComponentButton::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
}
