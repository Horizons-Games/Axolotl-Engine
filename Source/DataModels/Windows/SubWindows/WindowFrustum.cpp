#include "WindowFrustum.h"

#include "Application.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleCamera.h"
#include "ModuleScene.h"
#include "DataStructures/Quadtree.h"
#include "Scene/Scene.h"

WindowFrustum::WindowFrustum() : SubWindow("Frustum")
{
}

WindowFrustum::~WindowFrustum()
{
}

void WindowFrustum::DrawWindowContents()
{
	bool showAABBs = App->GetModule<ModuleDebugDraw>()->IsShowingBoundingBoxes();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		App->GetModule<ModuleDebugDraw>()->ShowBoundingBoxes(showAABBs);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum"};

	int currentFrustum = (int)App->GetModule<ModuleCamera>()->GetCamera()->GetFrustumMode();
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		App->GetModule<ModuleCamera>()->GetCamera()->SetFrustumMode((EFrustumMode)currentFrustum);
	}

	float vFrustum = App->GetModule<ModuleCamera>()->GetCamera()->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->GetModule<ModuleCamera>()->GetCamera()->SetFrustumOffset(vFrustum);
	}

	bool isQuadtreeFreezed = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->IsFreezed();
	if (ImGui::Checkbox("Freeze Quadtree", &isQuadtreeFreezed))
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->SetFreezedStatus(isQuadtreeFreezed);
	}

	int quadrantCapacity = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp)) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->SetQuadrantCapacity(quadrantCapacity);
		//TODO save values for future executions
	}

	float minQuadrantSideSize = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetMinQuadrantSideSize();
	if (ImGui::SliderFloat("Minimum quadrant side size", 
		&minQuadrantSideSize, 50.0, 500.0, "%.0f", ImGuiSliderFlags_AlwaysClamp)) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->SetMinQuadrantSideSize(minQuadrantSideSize);
		//TODO save values for future executions
	}

}

