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

	consoleContents.insert(std::end(consoleContents), std::begin(logContext->logLines), std::end(logContext->logLines));

	logContext->logLines.clear();

	for (std::string line : consoleContents)
	{
		ImGui::TextUnformatted(&line[0]);
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}
}
