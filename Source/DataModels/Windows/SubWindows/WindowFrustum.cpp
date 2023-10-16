#include "StdAfx.h"

#include "WindowFrustum.h"

#include "Application.h"
#include "DataStructures/Quadtree.h"
#include "ModuleScene.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleDebugDraw.h"
#include "Scene/Scene.h"
#include "Camera/Camera.h"

WindowFrustum::WindowFrustum() : SubWindow("Frustum")
{
}

WindowFrustum::~WindowFrustum()
{
}

void WindowFrustum::DrawWindowContents()
{
	ModuleDebugDraw* debug = App->GetModule<ModuleDebugDraw>();
	ModuleCamera* moduleCamera = App->GetModule<ModuleCamera>();
	Camera* camera = moduleCamera->GetCamera();

	ModuleScene* scene = App->GetModule<ModuleScene>();
	Scene* loadedScene = scene->GetLoadedScene();
	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();

	bool showAABBs = debug->IsShowingBoundingBoxes();
	bool showRB = debug->IsShowingRigidBody();
	bool showGrid = debug->IsShowingGrid();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		debug->ShowBoundingBoxes(showAABBs);
	}
	if (ImGui::Checkbox("Show Rigid bodies", &showRB))
	{
		debug->ShowRigidBody(showRB);
	}
	if (ImGui::Checkbox("Show Grid bodies", &showGrid))
	{
		debug->ShowGrid(showGrid);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };

	int currentFrustum = (int) camera->GetFrustumMode();

	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		camera->SetFrustumMode((EFrustumMode) currentFrustum);
	}

	float vFrustum = camera->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		camera->SetFrustumOffset(vFrustum);
	}

	bool isQuadtreeFreezed = rootQuadtree->IsFreezed();
	if (ImGui::Checkbox("Freeze Quadtree", &isQuadtreeFreezed))
	{
		rootQuadtree->SetFreezedStatus(isQuadtreeFreezed);
	}

	int quadrantCapacity = rootQuadtree->GetQuadrantCapacity();
	if (ImGui::SliderInt("Quadrant capacity", &quadrantCapacity, 1, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
	{
		rootQuadtree->SetQuadrantCapacity(quadrantCapacity);
		AXO_TODO("save values for future executions")
	}

	float minQuadrantSideSize = rootQuadtree->GetMinQuadrantSideSize();
	if (ImGui::SliderFloat(
			"Minimum quadrant side size", &minQuadrantSideSize, 50.0, 500.0, "%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		rootQuadtree->SetMinQuadrantSideSize(minQuadrantSideSize);
		AXO_TODO("save values for future executions")
	}
}
