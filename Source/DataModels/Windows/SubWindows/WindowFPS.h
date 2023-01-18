#pragma once

#include "SubWindow.h"

#include <vector>

class WindowFPS : public SubWindow
{
public:
	WindowFPS();
	~WindowFPS();

protected:
	void DrawWindowContents() override;

private:
	int fpsCaptures = 100;
	int timeCaptures = 100;
	int currentFpsIndex = 0;
	int currentTimeIndex = 0;
	std::vector<float> fpsHist;
	std::vector<float> timeHist;
};

