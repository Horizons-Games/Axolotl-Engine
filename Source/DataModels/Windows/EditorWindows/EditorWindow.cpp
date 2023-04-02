#include "EditorWindow.h"

EditorWindow::EditorWindow(const std::string& name) : Window(name), flags(ImGuiWindowFlags_None), focused(false)
{
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::Draw(bool& enabled)
{
	if (enabled)
	{
		if (ImGui::Begin(name.c_str(), &enabled, flags))
		{
			DrawWindowContents();
			focused = ImGui::IsWindowFocused();
		}
		ImGui::End();
	}
	// The call to ImGui::Begin can change the value of io_enabled
	// so using "else" will cause the window to remain focused for an extra frame
	if (!enabled)
	{
		focused = false;
	}
}
