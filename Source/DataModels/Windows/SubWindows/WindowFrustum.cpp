#include "WindowFrustum.h"
#include "Globals.h"
#include "imgui.h"
#include "Application.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleEngineCamera.h"
#include "ModuleScene.h"
#include "Quadtree.h"

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

	int currentFrustum = App->engineCamera->GetFrustumMode();
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		App->engineCamera->SetFrustumMode(currentFrustum);
	}

	float vFrustum = App->engineCamera->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->engineCamera->SetFrustumOffset(vFrustum);
	}

	int quadrantCapacity = App->scene->GetSceneQuadTree()->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		App->scene->GetSceneQuadTree()->SetQuadrantCapacity(quadrantCapacity);
		//TODO save values for future executions
	}

	float minCubeSize = App->scene->GetSceneQuadTree()->GetQuadrantSize();
	if (ImGui::SliderFloat("Minimum cube size", &minCubeSize, 50.0, 500.0, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->scene->GetSceneQuadTree()->SetMinCubeSize(minCubeSize);
		//TODO save values for future executions
	}

}

