#pragma once

#include "EditorWindow.h"
#include "Windows/SubWindows/SubWindow.h"

#include <vector>

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration();

protected:
	void DrawWindowContents() override;

private:
	std::vector<SubWindow*> collapsingSubWindows;

	ImVec2 GetStartingSize() const override
	{
		return ImVec2(900, 250);
	}
};

