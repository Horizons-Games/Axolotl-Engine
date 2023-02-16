#include "WindowFrustum.h"
#include "imgui.h"
#include "Application.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleEngineCamera.h"
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
	bool showAABBs = App->GetModuleDebugDraw()->IsShowingBoundingBoxes();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		App->GetModuleDebugDraw()->ShowBoundingBoxes(showAABBs);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum"};

	int currentFrustum = App->GetModuleEngineCamera()->GetFrustumMode();
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items,
			IM_ARRAYSIZE(listbox_items), 3))
	{
		App->GetModuleEngineCamera()->SetFrustumMode(currentFrustum);
	}

	float vFrustum = App->GetModuleEngineCamera()->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		App->GetModuleEngineCamera()->SetFrustumOffset(vFrustum);
	}

	bool isQuadtreeFreezed = App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->IsFreezed();
	if (ImGui::Checkbox("Freeze Quadtree", &isQuadtreeFreezed))
	{
		App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->SetFreezedStatus(isQuadtreeFreezed);
	}

	int quadrantCapacity = App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
	{
		App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->SetQuadrantCapacity(quadrantCapacity);
		//TODO save values for future executions
	}

	float minQuadrantSideSize = App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->GetMinQuadrantSideSize();
	if (ImGui::SliderFloat("Minimum quadrant side size", &minQuadrantSideSize, 50.0, 500.0,
			"%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		App->GetModuleScene()->GetLoadedScene()->GetSceneQuadTree()->SetMinQuadrantSideSize(minQuadrantSideSize);
		//TODO save values for future executions
	}

}

