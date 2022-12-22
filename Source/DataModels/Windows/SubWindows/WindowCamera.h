#pragma once

#include "SubWindow.h"

class WindowCamera : public SubWindow
{
public:
	WindowCamera();
	~WindowCamera();

protected:
	void DrawWindowContents() override;
};

