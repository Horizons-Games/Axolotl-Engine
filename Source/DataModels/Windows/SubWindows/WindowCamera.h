#pragma once

#include "SubWindow.h"

#include "Math/float4.h"

class WindowCamera : public SubWindow
{
public:
	WindowCamera();
	~WindowCamera() override;

protected:
	void DrawWindowContents() override;

private:
	float4 color;
};

