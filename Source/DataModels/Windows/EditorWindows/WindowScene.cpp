#pragma warning (disable: 4312)

#include "WindowScene.h"

#include "imgui.h"

#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEngineCamera.h"

WindowScene::WindowScene() : EditorWindow("Scene")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs;
}

WindowScene::~WindowScene()
{
}

void WindowScene::DrawWindowContents()
{
	ManageResize();
	ImGui::Image((void*)App->renderer->GetRenderedTexture(), 
		ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
}

void WindowScene::ManageResize()
{
	startingPos = ImGui::GetWindowContentRegionMin();
	startingPos.x += ImGui::GetWindowPos().x;
	startingPos.y += ImGui::GetWindowPos().y;

	ImVec2 availableRegion = ImGui::GetContentRegionAvail();
	bool widthChanged = currentWidth != availableRegion.x;
	bool heightChanged = currentHeight != availableRegion.y;
	if (widthChanged || heightChanged) // window was resized
	{ 
		App->engineCamera->SetAspectRatio(availableRegion.x / availableRegion.y);
		currentWidth = availableRegion.x;
		currentHeight = availableRegion.y;
	}
}
