#pragma once

#include "EditorWindow.h"

class SubWindow;

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration() override;

protected:
	void DrawWindowContents() override;


private:
	std::vector<std::unique_ptr<SubWindow> > collapsingSubWindows;
};

