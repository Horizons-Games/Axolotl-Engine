#include "WindowConsole.h"

#include <ranges>

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar;
	severityFilters[LogSeverity::INFO_LOG] = true;
	severityFilters[LogSeverity::VERBOSE_LOG] = false;
	severityFilters[LogSeverity::WARNING_LOG] = true;
	severityFilters[LogSeverity::ERROR_LOG] = true;
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::DrawWindowContents()
{
	consoleContents.insert(std::end(consoleContents), std::begin(logContext->logLines), std::end(logContext->logLines));
	logContext->logLines.clear();

	DrawOptionsMenu();
	DrawConsole();
}

void WindowConsole::DrawOptionsMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Filters"))
		{
			for (auto& filter : severityFilters)
			{
				const char* text = "";
				switch (filter.first)
				{
					case LogSeverity::INFO_LOG:
						text = "INFO";
						break;
					case LogSeverity::VERBOSE_LOG:
						text = "VERBOSE";
						break;
					case LogSeverity::WARNING_LOG:
						text = "WARNING";
						break;
					case LogSeverity::ERROR_LOG:
						text = "ERROR";
						break;
				}
				ImGui::MenuItem(text, nullptr, &filter.second);
			}
			ImGui::EndMenu();
		}

		if (ImGui::Button("Clear"))
		{
			consoleContents.clear();
		}

		ImGui::EndMenuBar();
	}
}

void WindowConsole::DrawConsole()
{
	auto linesAsString = consoleContents |
						 std::views::filter(
							 [this](const AxoLog::LogLine& logLine)
							 {
								 return severityFilters[logLine.severity];
							 }) |
						 std::views::transform(
							 [](const AxoLog::LogLine& logLine)
							 {
								 return logLine.ToSimpleString();
							 });

	for (std::string line : linesAsString)
	{
		ImGui::TextUnformatted(line.c_str());
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}
}
