#include "WindowConsole.h"
#include "Application.h"
#include "ModuleEditor.h"

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	//flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::DrawWindowContents()
{
	if (!firstDeployed)
	{
		ImGui::DockBuilderDockWindow("Console", App->editor->dock_down_id);
		//ImGui::SetNextWindowDockID(App->editor->dock_down_id, ImGuiCond_FirstUseEver);
		firstDeployed = true;
	}

	ImGui::SetWindowSize("Console log", ImVec2(900, 250), ImGuiCond_Once);

	std::string logInfo = "";

	for (int i = 0; i < engineLog->logLines.size(); ++i)
	{
		logInfo = engineLog->logLines[i].c_str();
		consoleContents.push_back(logInfo);
	}

	engineLog->logLines.clear();

	for (std::string line : consoleContents)
	{
		ImGui::TextUnformatted(&line[0]);
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
}
