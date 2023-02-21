#pragma once

#include "SubWindow.h"

class WindowRenderer : public SubWindow
{
public:
	WindowRenderer();
	~WindowRenderer();

protected:
	void DrawWindowContents() override;

private:
	static int bufferSize;

	std::string vertexShaderBuffer;
	std::string fragmentShaderBuffer;
};

