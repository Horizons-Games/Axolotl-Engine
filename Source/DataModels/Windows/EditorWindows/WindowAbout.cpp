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
	ImGui::TextWrapped("Description: nullptr");
	ImGui::Separator();
	ImGui::TextUnformatted("Author: Horizons Games");
	ImGui::TextUnformatted("License: MIT");    
}

