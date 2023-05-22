#include "WindowConsole.h"

#include <ranges>

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar;

	severityFilters[LogSeverity::INFO_LOG] = true;
	severityFilters[LogSeverity::VERBOSE_LOG] = true;
	severityFilters[LogSeverity::WARNING_LOG] = true;
	severityFilters[LogSeverity::ERROR_LOG] = true;

	severityColors[LogSeverity::INFO_LOG] = ImVec4(1.f, 1.f, 1.f, 1.f);	   // white
	severityColors[LogSeverity::VERBOSE_LOG] = ImVec4(0.f, 1.f, 1.f, 1.f); // cyan
	severityColors[LogSeverity::WARNING_LOG] = ImVec4(1.f, 1.f, 0.f, 1.f); // yellow
	severityColors[LogSeverity::ERROR_LOG] = ImVec4(1.f, 0.f, 0.f, 1.f);   // red
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
	auto linesFiltered = consoleContents | std::views::filter(
											   [this](const AxoLog::LogLine& logLine)
											   {
												   return severityFilters[logLine.severity];
											   });

	for (const AxoLog::LogLine& logLine : linesFiltered)
	{
		ImGui::TextColored(severityColors[logLine.severity], logLine.ToSimpleString().c_str());
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}
}
