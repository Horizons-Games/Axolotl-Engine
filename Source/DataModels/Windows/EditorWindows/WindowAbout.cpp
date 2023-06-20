#include "StdAfx.h"

#include "WindowAbout.h"

#include "Defines/ApplicationDefines.h"

WindowAbout::WindowAbout() : EditorWindow("About")
{
}

WindowAbout::~WindowAbout()
{
}

void WindowAbout::DrawWindowContents()
{
	ImGui::TextUnformatted(TITLE);
	ImGui::TextWrapped(
		"Description: Axolotl Engine is a game development tool built by a group of students at the UPC School");
	ImGui::Separator();
	ImGui::TextUnformatted("Author: Horizons Games");
	ImGui::TextUnformatted("License: MIT");
}
