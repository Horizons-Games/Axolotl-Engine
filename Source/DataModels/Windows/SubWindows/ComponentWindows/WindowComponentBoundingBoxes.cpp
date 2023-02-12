#include "WindowComponentBoundingBoxes.h"

#include "DataModels/Components/ComponentBoundingBoxes.h"

WindowComponentBoundingBoxes::WindowComponentBoundingBoxes(ComponentBoundingBoxes* component)
	: ComponentWindow("BOUNDING BOX", component)
{
}

void WindowComponentBoundingBoxes::DrawWindowContents()
{
	ComponentBoundingBoxes* asBB = static_cast<ComponentBoundingBoxes*>(this->component);

	if (asBB)
	{
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Bounding Box", &(asBB->drawBoundingBoxes));
	}
}
