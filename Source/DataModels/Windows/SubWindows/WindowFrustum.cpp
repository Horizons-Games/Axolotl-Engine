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
	ModuleDebugDraw* debug = App->GetModule<ModuleDebugDraw>();

	bool showAABBs = debug->IsShowingBoundingBoxes();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		debug->ShowBoundingBoxes(showAABBs);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum"};

	int currentFrustum = (int)App->GetModule<ModuleCamera>()->GetCamera()->GetFrustumMode();
	
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		camera->GetCamera()->SetFrustumMode((EFrustumMode)currentFrustum);
	}

	float vFrustum = camera->GetCamera()->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		camera->GetCamera()->SetFrustumOffset(vFrustum);
	}

	ModuleScene* scene = App->GetModule<ModuleScene>();

	bool isQuadtreeFreezed = scene->GetLoadedScene()->GetRootQuadtree()->IsFreezed();
	if (ImGui::Checkbox("Freeze Quadtree", &isQuadtreeFreezed))
	{
		scene->GetLoadedScene()->GetRootQuadtree()->SetFreezedStatus(isQuadtreeFreezed);
	}

	int quadrantCapacity = scene->GetLoadedScene()->GetRootQuadtree()->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp)) 
	{
		scene->GetLoadedScene()->GetRootQuadtree()->SetQuadrantCapacity(quadrantCapacity);
		//TODO save values for future executions
	}

	float minQuadrantSideSize = scene->GetLoadedScene()->GetRootQuadtree()->GetMinQuadrantSideSize();
	if (ImGui::SliderFloat("Minimum quadrant side size", 
		&minQuadrantSideSize, 50.0, 500.0, "%.0f", ImGuiSliderFlags_AlwaysClamp)) 
	{
		scene->GetLoadedScene()->GetRootQuadtree()->SetMinQuadrantSideSize(minQuadrantSideSize);
		//TODO save values for future executions
	}

}

