#pragma once

#include "SubWindow.h"

class WindowWindow : public SubWindow
{
public:
	WindowWindow();
	~WindowWindow();

protected:
	void DrawWindowContents() override;
};

