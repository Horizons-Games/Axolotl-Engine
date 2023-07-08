#include "StdAfx.h"

#include "SubWindow.h"

bool SubWindow::defaultEnabled = true;

SubWindow::SubWindow(const std::string& name) : Window(name), flags(ImGuiTreeNodeFlags_None)
{
}

SubWindow::~SubWindow()
{
}

void SubWindow::Draw(bool& enabled)
{
	if (ImGui::CollapsingHeader(name.c_str(), flags))
	{
		DrawWindowContents();
	}
	ImGui::Separator();
}
