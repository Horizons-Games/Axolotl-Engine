#include "WindowScene.h"

#include "imgui.h"

#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleEngineCamera.h"
#include "Modules/ModuleEditor.h"

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
	//ImGui::SetNextWindowDockID(App->editor->dock_main_id, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	ImGui::Image((void*)App->renderer->GetRenderedTexture(), 
	ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
}

void WindowScene::ManageResize()
{
	ImVec2 availableRegion = ImGui::GetContentRegionAvail();
	bool widthChanged = previousWidht != availableRegion.x;
	bool heightChanged = previousHeight != availableRegion.y;
	if (widthChanged || heightChanged) // window was resized
	{ 
		App->engineCamera->SetAspectRatio(availableRegion.x / availableRegion.y);
		previousWidht = availableRegion.x;
		previousHeight = availableRegion.y;
	}
}
