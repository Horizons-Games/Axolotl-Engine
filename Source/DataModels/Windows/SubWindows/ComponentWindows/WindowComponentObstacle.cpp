#include "WindowComponentObstacle.h"

#include "Components/ComponentObstacle.h"

WindowComponentObstacle::WindowComponentObstacle(ComponentObstacle* component) :
	ComponentWindow("OBSTACLE", component)
{
}

WindowComponentObstacle::~WindowComponentObstacle()
{
}

void WindowComponentObstacle::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentObstacle* asObstacle = static_cast<ComponentObstacle*>(component);

	if (asObstacle)
	{
	}
}
