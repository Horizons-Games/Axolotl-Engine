#pragma once

#include "SubWindow.h"

#include <string>

class WindowRenderer : public SubWindow
{
public:
	WindowRenderer();
	~WindowRenderer() override;

protected:
	void DrawWindowContents() override;

private:
	static int bufferSize;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;
};

