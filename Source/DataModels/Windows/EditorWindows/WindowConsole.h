#pragma once

#include "EditorWindow.h"

class WindowConsole : public EditorWindow
{
public:
	WindowConsole();
	~WindowConsole();

	void Output(const char* textToPrint);

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override
	{
		return ImVec2(900, 250);
	}

private:
	ImVector<const char*> consloneContents;

	std::vector<std::string> lines;
};

