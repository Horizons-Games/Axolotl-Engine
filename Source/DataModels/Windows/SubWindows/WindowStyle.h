#pragma once

#include "SubWindow.h"

class WindowStyle : public SubWindow
{
public:
	WindowStyle();
	~WindowStyle() override;

	void DrawWindowContents() override;
	void MainDockConfig();
	void ResetDockConfig();
};