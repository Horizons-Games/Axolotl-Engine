#pragma once

#include "SubWindow.h"

class WindowFrustum : public SubWindow
{
public:
	WindowFrustum();
	~WindowFrustum();

protected:
	void DrawWindowContents() override;
};

