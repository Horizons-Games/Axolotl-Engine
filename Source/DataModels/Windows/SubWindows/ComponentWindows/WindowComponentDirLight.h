#pragma once

#include "WindowComponentLight.h"

class ComponentDirLight;

class WindowComponentDirLight : public WindowComponentLight
{
public:
	WindowComponentDirLight(ComponentDirLight* component);
	~WindowComponentDirLight() override;

protected:
	void DrawWindowContents() override;
};

