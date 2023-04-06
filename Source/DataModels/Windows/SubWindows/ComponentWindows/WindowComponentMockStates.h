#pragma once

#include "ComponentWindow.h"

class ComponentMockState;

class WindowComponentMockStates :
	public ComponentWindow
{
public:
	WindowComponentMockStates(ComponentMockState* component);
	~WindowComponentMockStates() override;

protected:
	void DrawWindowContents() override;

};


