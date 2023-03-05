#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowAbout : public EditorWindow
{
public:
	WindowAbout();
	~WindowAbout() override;

protected:
	void DrawWindowContents() override;
};

