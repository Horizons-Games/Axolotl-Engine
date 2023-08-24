#pragma once

#include "ComponentWindow.h"

class ComponentAgent;

class WindowComponentAgent : public ComponentWindow
{
public:
	WindowComponentAgent(ComponentAgent* component);
	~WindowComponentAgent() override;

protected:
	void DrawWindowContents() override;
};
