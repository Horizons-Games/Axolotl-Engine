#pragma once

#include "ComponentWindow.h"

class ComponentDirLight;

class WindowComponentDirLight : public ComponentWindow
{
public:
	WindowComponentDirLight(ComponentDirLight* component);
	~WindowComponentDirLight() override = default;

protected:
	void DrawWindowContents() override;
};

