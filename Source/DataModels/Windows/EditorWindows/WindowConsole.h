#pragma once
#include "EditorWindow.h"

#include "AxoLog.h"

#include <map>
#include <vector>

class WindowConsole : public EditorWindow
{
public:
	WindowConsole();
	~WindowConsole() override;

protected:
	void DrawWindowContents() override;

private:
	void DrawOptionsMenu();
	void DrawConsole();

	std::vector<AxoLog::LogLine> consoleContents;
	std::map<LogSeverity, bool> severityFilters;
	std::map<LogSeverity, ImVec4> severityColors;
};
