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
	bool showAABBs = App->debug->IsShowingBoundingBoxes();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		App->debug->ShowBoundingBoxes(showAABBs);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum"};

	int currentFrustum = (int)App->engineCamera->GetCamera()->GetFrustumMode();
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		App->engineCamera->GetCamera()->SetFrustumMode((EFrustumMode)currentFrustum);
	}

	float vFrustum = App->engineCamera->GetCamera()->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->engineCamera->GetCamera()->SetFrustumOffset(vFrustum);
	}

	bool isQuadtreeFreezed = App->scene->GetLoadedScene()->GetSceneQuadTree()->IsFreezed();
	if (ImGui::Checkbox("Freeze Quadtree", &isQuadtreeFreezed))
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->SetFreezedStatus(isQuadtreeFreezed);
	}

	int quadrantCapacity = App->scene->GetLoadedScene()->GetSceneQuadTree()->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp)) 
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->SetQuadrantCapacity(quadrantCapacity);
		//TODO save values for future executions
	}

	float minQuadrantSideSize = App->scene->GetLoadedScene()->GetSceneQuadTree()->GetMinQuadrantSideSize();
	if (ImGui::SliderFloat("Minimum quadrant side size", 
		&minQuadrantSideSize, 50.0, 500.0, "%.0f", ImGuiSliderFlags_AlwaysClamp)) 
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->SetMinQuadrantSideSize(minQuadrantSideSize);
		//TODO save values for future executions
	}

}

