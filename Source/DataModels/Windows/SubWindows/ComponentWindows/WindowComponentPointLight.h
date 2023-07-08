#pragma once

#include "WindowComponentLight.h"

class ComponentPointLight;

class WindowComponentPointLight : public WindowComponentLight
{
public:
	WindowComponentPointLight(ComponentPointLight* component);
	~WindowComponentPointLight() override;

protected:
	void DrawWindowContents() override;
};
