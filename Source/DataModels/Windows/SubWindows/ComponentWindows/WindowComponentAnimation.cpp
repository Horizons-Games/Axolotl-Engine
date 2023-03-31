#include "WindowComponentAnimation.h"

#include "Components/ComponentAnimation.h"

#include "Application.h"


WindowComponentAnimation::WindowComponentAnimation(ComponentAnimation* component) :
	ComponentWindow("ANIMATION", component)
{
}

WindowComponentAnimation::~WindowComponentAnimation()
{
}

void WindowComponentAnimation::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
}
