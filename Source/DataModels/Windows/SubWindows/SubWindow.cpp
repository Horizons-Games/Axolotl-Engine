#include "SubWindow.h"

bool SubWindow::defaultEnabled = true;

SubWindow::SubWindow(const std::string& name) : Window(name), flags(ImGuiTreeNodeFlags_None)
{
	enabled = defaultEnabled;
}

SubWindow::~SubWindow()
{
}

void SubWindow::Draw()
{
	if (ImGui::CollapsingHeader(name.c_str(), flags))
	{
		DrawWindowContents();
	}
	ImGui::Separator();
}
