#include "StdAfx.h"

#include "WindowConsole.h"

namespace
{
const std::vector<size_t> consoleLineLengths{ 10U, 25U, 50U, 100U };
std::optional<size_t> selectedMaxLinesIndex = std::nullopt;
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
	const char* noLimitButtonText = "No limit";

	// Align the buttons to the right
	ImGuiStyle& style = ImGui::GetStyle();

	float buttonWidth = ImGui::CalcTextSize("XXX").x + style.FramePadding.x * 2.0f;
	float buttonContainerWidth = (buttonWidth + style.ItemSpacing.x) * numButtons;

	float noLimitButtonWidth = ImGui::CalcTextSize(noLimitButtonText).x + style.FramePadding.x * 2.0f;
	float noLimitButtonContainerWidth = noLimitButtonWidth + style.ItemSpacing.x;

	float textWidth = ImGui::CalcTextSize(limitText).x + style.FramePadding.x * 2.0f;

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - noLimitButtonContainerWidth - buttonContainerWidth - textWidth);

	ImGui::TextUnformatted(limitText);
	ImGui::SameLine();

	for (int i = 0; i < numButtons; ++i)
	{
		ImGui::PushID(i);

		// store the condition in case it changes during draw
		bool selected = selectedMaxLinesIndex.has_value() && i == selectedMaxLinesIndex.value();
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

	// store the condition in case it changes during draw
	bool selected = !selectedMaxLinesIndex.has_value();

	if (selected)
	{
		ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	}

	if (ImGui::Button(noLimitButtonText, ImVec2(noLimitButtonWidth, 0)))
	{
		selectedMaxLinesIndex = std::nullopt;
	}

	if (selected)
	{
		ImGui::PopStyleColor();
	}
}

void WindowConsole::DrawConsole()
{
	size_t linesToDraw =
		selectedMaxLinesIndex.has_value() ? consoleLineLengths[selectedMaxLinesIndex.value()] : consoleContents.size();
	auto linesFiltered = consoleContents |
						 std::views::filter(
							 [this](const AxoLog::LogLine& logLine)
							 {
								 return severityFilters[logLine.severity];
							 }) |
						 // Reverse to take the last N elements, then reverse again to maintain line order
						 std::views::reverse | std::views::take(linesToDraw) | std::views::reverse;

	// Define the position from which text will start to wrap, which will be the end of the console window
	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);

	for (const AxoLog::LogLine& logLine : linesFiltered)
	{
		ImGui::TextColored(severityColors[logLine.severity], logLine.ToSimpleString().c_str());
		// Add spacing between each line
		ImGui::Dummy(ImVec2(0.0f, 0.5f));
	}

	ImGui::PopTextWrapPos();

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}
}
