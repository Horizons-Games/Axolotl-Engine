#pragma warning (disable: 4312)

#include "WindowScene.h"

#include "Application.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleCamera.h"

WindowScene::WindowScene() : EditorWindow("Scene"), texture(0),
	currentWidth(0), currentHeight(0)
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
	auto viewportOffset = ImGui::GetCursorPos(); // include tab bar
	
	availableRegion = ImGui::GetContentRegionAvail();
	bool widthChanged = currentWidth != availableRegion.x;
	bool heightChanged = currentHeight != availableRegion.y;
	if (widthChanged || heightChanged) // window was resized
	{ 
		App->engineCamera->GetCamera()->SetAspectRatio(availableRegion.x / availableRegion.y);
		currentWidth = availableRegion.x;
		currentHeight = availableRegion.y;
	}
	
	auto windowSize = ImGui::GetWindowSize();

	ImVec2 minBounds = ImGui::GetWindowPos();
	minBounds.x += viewportOffset.x;
	minBounds.y += viewportOffset.y;
	
	ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y};
	
	viewportBounds[0] = { minBounds.x, minBounds.y };
	viewportBounds[1] = { maxBounds.x, maxBounds.y };
}
