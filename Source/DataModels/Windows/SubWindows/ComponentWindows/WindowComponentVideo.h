#pragma once
#include "ComponentWindow.h"

class ComponentVideo;
class WindowVideoInput;


class WindowComponentVideo : public ComponentWindow
{
public:
	WindowComponentVideo(ComponentVideo* component);
	~WindowComponentVideo() override;

protected:
	void DrawWindowContents() override;

private:
	std::unique_ptr<WindowVideoInput> inputVideo;
};