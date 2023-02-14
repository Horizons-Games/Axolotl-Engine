#pragma once

#include "ComponentWindow.h"

class ComponentLight;

class WindowComponentLight : public ComponentWindow
{
public:
	WindowComponentLight(ComponentLight* component);
	~WindowComponentLight() override = default;

protected:
	void DrawWindowContents() override;
};

