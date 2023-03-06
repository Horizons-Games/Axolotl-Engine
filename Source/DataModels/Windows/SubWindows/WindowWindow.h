#pragma once

#include "SubWindow.h"

class WindowWindow : public SubWindow
{
public:
	WindowWindow();
	~WindowWindow() override;

protected:
	void DrawWindowContents() override;
};

