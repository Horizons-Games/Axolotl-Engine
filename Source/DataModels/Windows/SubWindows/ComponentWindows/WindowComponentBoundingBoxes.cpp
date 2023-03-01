#include "WindowComponentBoundingBoxes.h"

#include "DataModels/Components/ComponentMeshRenderer.h"

WindowComponentBoundingBoxes::WindowComponentBoundingBoxes(ComponentMeshRenderer* component)
	: ComponentWindow("BOUNDING BOX", component)
{
}

WindowComponentBoundingBoxes::~WindowComponentBoundingBoxes()
{
}

void WindowComponentBoundingBoxes::DrawWindowContents()
{
	ComponentMeshRenderer* asBB = static_cast<ComponentMeshRenderer*>(component);

	if (asBB)
	{
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Bounding Box", &(asBB->drawBoundingBoxes));
	}
}
