#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowAbout : public EditorWindow
{
public:
	WindowAbout();
	~WindowAbout();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
	bool firstDeployed = false;
};

inline ImVec2 WindowAbout::GetStartingSize() const
{
	return ImVec2(900, 250);
}
