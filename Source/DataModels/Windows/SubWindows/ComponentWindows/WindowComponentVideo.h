#pragma once
#include "ComponentWindow.h"

class ComponentVideo;
class WindowSpriteInput;

class WindowComponentVideo : public ComponentWindow
{
public:
	WindowComponentVideo(ComponentVideo* component);
	~WindowComponentVideo() override;

protected:
	void DrawWindowContents() override;

};