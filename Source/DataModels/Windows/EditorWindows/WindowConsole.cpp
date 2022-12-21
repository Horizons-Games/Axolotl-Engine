#include "WindowConsole.h"

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::DrawWindowContents()
{
	ImGui::SetWindowSize("Console log", ImVec2(900, 250), ImGuiCond_Once);

	std::string logInfo = "";

	for (int i = 0; i < engineLog->logLines.size(); ++i)
	{
		logInfo = engineLog->logLines[i].c_str();
		lines.push_back(logInfo);
	}

	engineLog->logLines.clear();

	for (std::string line : lines)
	{
		ImGui::TextUnformatted(&line[0]);
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	/*for (int i = 0; i < consloneContents.size(); ++i)
	{
		const char* line = consloneContents[i];
		ImGui::TextUnformatted(line);
	}*/
}

void WindowConsole::Output(const char* textToPrint)
{
	//APP_LOG(textToPrint);
	consloneContents.push_back(_strdup(textToPrint));
}
