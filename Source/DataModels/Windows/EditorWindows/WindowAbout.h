#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowAbout : public EditorWindow
{
public:
	WindowAbout();
	~WindowAbout();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override
	{
		return ImVec2(900, 250);
	}
};

