#pragma once

#include "SubWindow.h"

class WindowInput : public SubWindow
{
public:
	WindowInput();
	~WindowInput() override;

protected:
	void DrawWindowContents() override;
};
