#include "WindowConsole.h"

#include <ranges>

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

	auto linesAsString = std::ranges::transform_view(logContext->logLines,
													 [](const AxoLog::LogLine& logLine)
													 {
														 return logLine.ToSimpleString();
													 });

	consoleContents.insert(std::end(consoleContents), std::begin(linesAsString), std::end(linesAsString));

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
