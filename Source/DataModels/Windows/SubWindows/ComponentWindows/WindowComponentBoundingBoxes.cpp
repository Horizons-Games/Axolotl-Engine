#include "WindowComponentBoundingBoxes.h"

#include "DataModels/Components/ComponentBoundingBoxes.h"

WindowComponentBoundingBoxes::WindowComponentBoundingBoxes(const std::weak_ptr<ComponentBoundingBoxes>& component)
	: ComponentWindow("BOUNDING BOX", component)
{
}

void WindowComponentBoundingBoxes::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentBoundingBoxes> asSharedBB =
		std::dynamic_pointer_cast<ComponentBoundingBoxes>(this->component.lock());

	if (asSharedBB)
	{
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Bounding Box", &(asSharedBB->drawBoundingBoxes));
	}
}
