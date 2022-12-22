#include "WindowConfiguration.h"

#include "Windows/SubWindows/WindowFPS.h"
#include "Windows/SubWindows/WindowRenderer.h"

WindowConfiguration::WindowConfiguration() : EditorWindow("Configuration")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	collapsingSubWindows.push_back(std::make_unique<WindowFPS>());
	collapsingSubWindows.push_back(std::make_unique<WindowRenderer>());
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
