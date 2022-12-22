#pragma once

#include "EditorWindow.h"

class WindowConsole : public EditorWindow
{
public:
	WindowConsole();
	~WindowConsole();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	std::vector<std::string> consoleContents;
};

inline ImVec2 WindowConsole::GetStartingSize() const
{
	return ImVec2(900, 250);
}
