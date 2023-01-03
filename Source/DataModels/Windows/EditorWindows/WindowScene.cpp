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

void WindowScene::Update()
{
	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
		{
			ImGui::CaptureMouseFromApp(true);
			ImGuiIO& io = ImGui::GetIO();
			/*float2 mouse_pos_normalized;
			mouse_pos_normalized.x = -1 + 2 * std::max(-1.0f, std::min((io.MousePos.x - framebuffer_position.x) / ( availableRegion.x), 1.0f));
			mouse_pos_normalized.y = 1 - 2 * std::max(-1.0f, std::min((io.MousePos.y - framebuffer_position.y) / ( availableRegion.y), 1.0f));
			App->camera->CalculateFrustumNearestObject(mouse_pos_normalized);*/
		}
		ImGui::CaptureMouseFromApp(false);
	}
}

void WindowScene::DrawWindowContents()
{
	ManageResize();
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
