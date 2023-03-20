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
	ImGui::TextWrapped("Description: Axolotl Engine is a game development tool developed by a group of students at the UPC SCHOOL");
	ImGui::Separator();
	ImGui::TextUnformatted("Author: Horizons Games");
	ImGui::TextUnformatted("License: MIT");
}
