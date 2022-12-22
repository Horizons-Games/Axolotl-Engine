#include "WindowConfiguration.h"

#include "Windows/SubWindows/WindowFPS.h"
#include "Windows/SubWindows/WindowRenderer.h"
#include "Windows/SubWindows/WindowHardware.h"
#include "Windows/SubWindows/WindowWindow.h"

WindowConfiguration::WindowConfiguration() : EditorWindow("Configuration")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	collapsingSubWindows.push_back(std::make_unique<WindowFPS>());
	collapsingSubWindows.push_back(std::make_unique<WindowRenderer>());
	collapsingSubWindows.push_back(std::make_unique<WindowHardware>());
	collapsingSubWindows.push_back(std::make_unique<WindowWindow>());
}

WindowConfiguration::~WindowConfiguration()
{
	collapsingSubWindows.clear();
}

void WindowConfiguration::DrawWindowContents()
{
	for (int i = 0; i < collapsingSubWindows.size(); ++i)
		collapsingSubWindows[i]->Draw();
}
