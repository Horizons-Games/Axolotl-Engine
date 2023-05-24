#include "WindowConfiguration.h"

#include "Windows/SubWindows/WindowCamera.h"
#include "Windows/SubWindows/WindowFPS.h"
#include "Windows/SubWindows/WindowFrustum.h"
#include "Windows/SubWindows/WindowHardware.h"
#include "Windows/SubWindows/WindowRenderer.h"
#include "Windows/SubWindows/WindowWindow.h"

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
	for (int i = 0; i < collapsingSubWindows.size(); ++i)
	{
		collapsingSubWindows[i]->Draw();
	}
}
