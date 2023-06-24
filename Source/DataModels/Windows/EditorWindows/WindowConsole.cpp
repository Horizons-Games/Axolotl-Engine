#include "StdAfx.h"

#include "WindowConsole.h"

namespace
{
const std::vector<size_t> consoleLineLengths{ 10U, 25U, 50U, 100U };
size_t selectedMaxLinesIndex = 2U;
} // namespace

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar;

	severityFilters[LogSeverity::INFO_LOG] = true;
	severityFilters[LogSeverity::VERBOSE_LOG] = true;
	severityFilters[LogSeverity::DEBUG_LOG] = true;
	severityFilters[LogSeverity::WARNING_LOG] = true;
	severityFilters[LogSeverity::ERROR_LOG] = true;

	severityColors[LogSeverity::INFO_LOG] = ImVec4(1.f, 1.f, 1.f, 1.f);		 // white
	severityColors[LogSeverity::VERBOSE_LOG] = ImVec4(0.f, 1.f, 1.f, 1.f);	 // cyan
	severityColors[LogSeverity::DEBUG_LOG] = ImVec4(0.22f, 1.f, 0.08f, 1.f); // lime
	severityColors[LogSeverity::WARNING_LOG] = ImVec4(1.f, 1.f, 0.f, 1.f);	 // yellow
	severityColors[LogSeverity::ERROR_LOG] = ImVec4(1.f, 0.f, 0.f, 1.f);	 // red
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::DrawWindowContents()
{
	consoleContents.insert(std::end(consoleContents), std::begin(logContext->logLines), std::end(logContext->logLines));
	logContext->logLines.clear();
	LimitConsoleContents();

	DrawOptionsMenu();
	DrawConsole();
}

void WindowConsole::LimitConsoleContents()
{
	size_t consoleLines = consoleContents.size();
	size_t maxConsoleLines = consoleLineLengths[selectedMaxLinesIndex];
	if (maxConsoleLines < consoleLines)
	{
		auto contentsBegin = std::begin(consoleContents);
		// the first parameter of std::advance is an input/output one
		// meaning it will be modified by the method
		std::advance(contentsBegin, consoleLines - maxConsoleLines);
		consoleContents.erase(std::begin(consoleContents), contentsBegin);
	}
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
					case LogSeverity::DEBUG_LOG:
						text = "DEBUG";
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

		DrawMaxLengthSelection();

		ImGui::EndMenuBar();
	}
}

void WindowConsole::DrawMaxLengthSelection()
{
	int numButtons = static_cast<int>(consoleLineLengths.size());
	const char* limitText = "Limit console lines:";

	// Align the buttons to the right
	ImGuiStyle& style = ImGui::GetStyle();

	float buttonWidth = ImGui::CalcTextSize("XXX").x + style.FramePadding.x * 2.0f;
	float buttonsWidth = (buttonWidth + style.ItemSpacing.x) * numButtons;

	float textWidth = ImGui::CalcTextSize(limitText).x + style.FramePadding.x * 2.0f;

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonsWidth - textWidth);

	ImGui::TextUnformatted(limitText);
	ImGui::SameLine();

	for (int i = 0; i < numButtons; ++i)
	{
		ImGui::PushID(i);

		// store the condition in case it changes during draw
		bool selected = i == selectedMaxLinesIndex;
		if (selected)
		{
			ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
			ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
		}

		if (ImGui::Button(std::to_string(consoleLineLengths[i]).c_str(), ImVec2(buttonWidth, 0)))
		{
			selectedMaxLinesIndex = i;
		}

		if (selected)
		{
			ImGui::PopStyleColor();
		}

		ImGui::PopID();

		ImGui::SameLine();
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
