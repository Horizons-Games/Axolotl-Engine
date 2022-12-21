#include "WindowConfiguration.h"

#include "Windows/SubWindows/WindowRenderer.h"

WindowConfiguration::WindowConfiguration() : EditorWindow("Configuration")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	collapsingSubWindows.push_back(new WindowRenderer());
}

WindowConfiguration::~WindowConfiguration()
{
	for (SubWindow* window : collapsingSubWindows)
		delete window;
	collapsingSubWindows.clear();
}

void WindowConfiguration::DrawWindowContents()
{
	for (SubWindow* subWindow : collapsingSubWindows)
		subWindow->Draw();
}
