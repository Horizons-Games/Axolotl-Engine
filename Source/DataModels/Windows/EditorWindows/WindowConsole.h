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

private:
	ImVector<const char*> consloneContents;
};

