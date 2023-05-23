#include "DataModels/Windows/PopUpWindows/PopUpWindow.h"

#include "imgui.h"

PopUpWindow::PopUpWindow(const std::string& name) : Window(name)
{
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
	enabled =
		ImGui::BeginPopupModal(name.c_str(),
							   &enabled,
							   ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoMove |
								   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	if (enabled)
	{
		DrawWindowContents();
		ImGui::EndPopup();
	}
}
