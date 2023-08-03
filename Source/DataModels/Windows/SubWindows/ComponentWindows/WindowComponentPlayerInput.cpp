#include "StdAfx.h"

#include "WindowComponentPlayerInput.h"

#include "Components/ComponentPlayerInput.h"

#include "Application.h"

WindowComponentPlayerInput::WindowComponentPlayerInput(ComponentPlayerInput* component) :
	ComponentWindow("PLAYER INPUT", component)
{
}

WindowComponentPlayerInput::~WindowComponentPlayerInput()
{
}

void WindowComponentPlayerInput::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ImGui::Text("Player Input");
}
