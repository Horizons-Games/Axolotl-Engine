#pragma once

#include "SubWindow.h"

class WindowFrustum : public SubWindow
{
public:
	WindowFrustum();
	~WindowFrustum() override;

protected:
	void DrawWindowContents() override;
};

