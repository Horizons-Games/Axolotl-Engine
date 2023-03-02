#include "WindowComponentBoundingBoxes.h"

#include "DataModels/Components/Component.h"
#include "DataModels/GameObject/GameObject.h"

WindowComponentBoundingBoxes::WindowComponentBoundingBoxes(Component* component)
	: ComponentWindow("BOUNDING BOX", component)
{
}

WindowComponentBoundingBoxes::~WindowComponentBoundingBoxes()
{
}

void WindowComponentBoundingBoxes::DrawWindowContents()
{
	GameObject* go = component->GetOwner();

	if (go)
	{
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Bounding Box", &(go->drawBoundingBoxes));
	}
}
