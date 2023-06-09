#include "DataModels/Windows/PopUpWindows/PopUpWindow.h"

PopUpWindow::PopUpWindow(const std::string& name) : Window(name)
{
	flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize;
}

PopUpWindow::~PopUpWindow()
{
}

void PopUpWindow::Draw(bool& enabled)
{
	if (enabled)
	{
		ImGui::OpenPopup(name.c_str());
		// Center the PopUp
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(
			ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	}
	enabled = ImGui::BeginPopupModal(name.c_str(), &enabled, flags);
	if (enabled)
	{
		DrawWindowContents();
		ImGui::EndPopup();
	}
}
