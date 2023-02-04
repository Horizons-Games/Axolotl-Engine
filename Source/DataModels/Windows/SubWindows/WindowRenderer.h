#pragma once

#include "SubWindow.h"

#include <string>

class WindowRenderer : public SubWindow
{
public:
	WindowRenderer();
	~WindowRenderer();

protected:
	void DrawWindowContents() override;

private:
	static int bufferSize;
	bool initialized;
	std::vector<std::string>vertexShaderBuffers;
	std::vector<std::string>fragmentShaderBuffer;
};

