#pragma once
#include "ComponentWindow.h"

class ComponentPlayer;

class WindowComponentPlayer :
    public ComponentWindow
{
public:
	WindowComponentPlayer(ComponentPlayer* component);
	~WindowComponentPlayer() override;

protected:
	void DrawWindowContents() override;

};

