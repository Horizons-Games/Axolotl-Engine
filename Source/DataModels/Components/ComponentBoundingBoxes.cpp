#include "ComponentBoundingBoxes.h"

#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "Application.h"
#include "ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"
#include "Scene.h"

#include "imgui.h"

ComponentBoundingBoxes::ComponentBoundingBoxes(bool active, GameObject* owner)
	: Component(ComponentType::BOUNDINGBOX, active, owner, false)
{
	localAABB = { {0 ,0, 0}, {0, 0, 0} };
	encapsuledAABB = localAABB;
	objectOBB = { localAABB };
	drawBoundingBoxes = false;
}

void ComponentBoundingBoxes::CalculateBoundingBoxes() 
{
	ComponentTransform* transform = (ComponentTransform*)((Component*)this)->GetOwner()->GetComponent(ComponentType::TRANSFORM);
	objectOBB = OBB(localAABB);
	objectOBB.Transform(transform->GetGlobalMatrix());
	encapsuledAABB = objectOBB.MinimalEnclosingAABB();
}

void ComponentBoundingBoxes::Draw()
{
	if (drawBoundingBoxes) App->debug->DrawBoundingBox(GetObjectOBB());
}


void ComponentBoundingBoxes::Display()
{
	ImGui::Text("BOUNDING BOXES");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("BoundingTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		ImGui::Checkbox("", &drawBoundingBoxes);

		ImGui::EndTable();
		ImGui::Separator();
	}

}