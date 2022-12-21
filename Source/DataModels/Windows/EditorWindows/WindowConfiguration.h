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
};

