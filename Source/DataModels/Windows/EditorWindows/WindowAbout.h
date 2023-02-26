#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowAbout : public EditorWindow
{
public:
	WindowAbout();
	~WindowAbout();

protected:
	void DrawWindowContents() override;
};

