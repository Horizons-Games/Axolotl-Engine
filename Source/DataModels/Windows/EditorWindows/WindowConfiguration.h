#pragma once

#include "EditorWindow.h"

#include <vector>
#include <memory>

class SubWindow;

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration();

protected:
	void DrawWindowContents() override;


private:
	std::vector<std::unique_ptr<SubWindow> > collapsingSubWindows;
};

