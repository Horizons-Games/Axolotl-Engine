#pragma once

#include "SubWindow.h"

class WindowCamera : public SubWindow
{
public:
	WindowCamera();
	~WindowCamera() override;

protected:
	void DrawWindowContents() override;
};

