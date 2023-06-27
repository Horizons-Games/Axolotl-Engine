#include "WindowComponentAgent.h"

#include "Components/ComponentAgent.h"

WindowComponentAgent::WindowComponentAgent(ComponentAgent* component) : ComponentWindow("AGENT", component)
{
}

WindowComponentAgent::~WindowComponentAgent()
{
}

void WindowComponentAgent::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentAgent* asAgent = static_cast<ComponentAgent*>(component);

	if (asAgent)
	{
	}
}
