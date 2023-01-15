#include "ComponentBoundingBoxes.h"

#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "Application.h"
#include "ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"
#include "Scene.h"
#include "FileSystem/Json.h"

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

void ComponentBoundingBoxes::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = "Bounding";
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	meta["AABBMin_X"] = (float)localAABB.minPoint.x;
	meta["AABBMin_Y"] = (float)localAABB.minPoint.y;
	meta["AABBMin_Z"] = (float)localAABB.minPoint.z;

	meta["AABBMax_X"] = (float)localAABB.maxPoint.x;
	meta["AABBMax_Y"] = (float)localAABB.maxPoint.y;
	meta["AABBMax_Z"] = (float)localAABB.maxPoint.z;
	
}

void ComponentBoundingBoxes::LoadOptions(Json& meta)
{
	// Do not delete these
	type = (ComponentType)(int) meta["type"];
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	localAABB = { { (float)meta["AABBMin_X"], (float)meta["AABBMin_Y"], (float)meta["AABBMin_Z"] }, { (float)meta["AABBMax_X"], (float)meta["AABBMax_Y"], (float)meta["AABBMax_Z"] } };
}