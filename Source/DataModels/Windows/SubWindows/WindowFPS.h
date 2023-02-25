#pragma once

#include "SubWindow.h"

#include <vector>

class WindowFPS : public SubWindow
{
public:
	WindowFPS();
	~WindowFPS() override;

protected:
	void DrawWindowContents() override;

private:
	int fpsCaptures;
	int timeCaptures;
	int currentFpsIndex;
	int currentTimeIndex;
	std::vector<float> fpsHist;
	std::vector<float> timeHist;
};

