#include "WindowAbout.h"

WindowAbout::WindowAbout() : EditorWindow("About")
{
}

WindowAbout::~WindowAbout()
{
}

void WindowAbout::DrawWindowContents()
{
	ImGui::TextUnformatted(TITLE);
	ImGui::TextWrapped("Description");
	ImGui::Separator();
	ImGui::TextUnformatted("Author: ");
	ImGui::TextUnformatted("License: MIT");
}
