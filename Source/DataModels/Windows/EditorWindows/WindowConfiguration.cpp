#include "WindowConfiguration.h"
#include "Application.h"
#include "ModuleEditor.h"

#include "Windows/SubWindows/WindowFPS.h"
#include "Windows/SubWindows/WindowRenderer.h"
#include "Windows/SubWindows/WindowHardware.h"
#include "Windows/SubWindows/WindowWindow.h"
#include "Windows/SubWindows/WindowCamera.h"
#include "Windows/SubWindows/WindowFrustum.h"

WindowConfiguration::WindowConfiguration() : EditorWindow("Configuration")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	collapsingSubWindows.push_back(std::make_unique<WindowFPS>());
	collapsingSubWindows.push_back(std::make_unique<WindowRenderer>());
	collapsingSubWindows.push_back(std::make_unique<WindowHardware>());
	collapsingSubWindows.push_back(std::make_unique<WindowWindow>());
	collapsingSubWindows.push_back(std::make_unique<WindowCamera>());
	collapsingSubWindows.push_back(std::make_unique<WindowFrustum>());
}

WindowConfiguration::~WindowConfiguration()
{
	collapsingSubWindows.clear();
}

void WindowConfiguration::DrawWindowContents()
{
	//ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowDockID(App->editor->dock_main_id, ImGuiCond_FirstUseEver);
	for (int i = 0; i < collapsingSubWindows.size(); ++i)
		collapsingSubWindows[i]->Draw();
}
