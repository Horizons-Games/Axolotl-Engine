#pragma once
#include "ComponentWindow.h"

class ComponentAnimation;

class WindowComponentAnimation :
	public ComponentWindow
{
public:
	WindowComponentAnimation(ComponentAnimation* component);
	~WindowComponentAnimation() override;

protected:
	void DrawWindowContents() override;

};

