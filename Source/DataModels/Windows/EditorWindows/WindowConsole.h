#pragma once
#include "EditorWindow.h"

#include <map>
#include <deque>

class WindowConsole : public EditorWindow
{
public:
	WindowConsole();
	~WindowConsole() override;

protected:
	void DrawWindowContents() override;

private:
	void LimitConsoleContents();
	void DrawOptionsMenu();
	void DrawConsole();

	std::deque<AxoLog::LogLine> consoleContents;
	std::map<LogSeverity, bool> severityFilters;
	std::map<LogSeverity, ImVec4> severityColors;
};
