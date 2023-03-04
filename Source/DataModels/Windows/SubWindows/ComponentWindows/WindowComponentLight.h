#pragma once

#include "ComponentWindow.h"

class ComponentLight;

class WindowComponentLight : public ComponentWindow
{
public:
	WindowComponentLight(ComponentLight* component);
	~WindowComponentLight() override;

protected:
	void DrawWindowContents() override;
};

