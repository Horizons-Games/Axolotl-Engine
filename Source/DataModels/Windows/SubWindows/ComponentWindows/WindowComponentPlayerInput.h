#pragma once
#include "ComponentWindow.h"

class ComponentPlayerInput;

class WindowComponentPlayerInput : public ComponentWindow
{
public:
	WindowComponentPlayerInput(ComponentPlayerInput* component);
	~WindowComponentPlayerInput() override;

protected:
	void DrawWindowContents() override;
};
