#pragma once

#include "EditorWindow.h"

class WindowConsole : public EditorWindow
{
public:
	WindowConsole();
	~WindowConsole();

protected:
	void DrawWindowContents() override;

private:
	std::vector<std::string> consoleContents;
};

