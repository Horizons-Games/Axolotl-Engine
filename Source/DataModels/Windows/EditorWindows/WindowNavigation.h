#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowNavigation : public EditorWindow
{
public:
	WindowNavigation();
	~WindowNavigation() override;

protected:
	void DrawWindowContents() override;
};
