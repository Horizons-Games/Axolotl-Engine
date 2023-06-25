#pragma once
#include "WindowComponentLight.h"

class ComponentAreaLight;

class WindowComponentAreaLight : public WindowComponentLight
{
public:
	WindowComponentAreaLight(ComponentAreaLight* component);
	~WindowComponentAreaLight() override;

protected:
	void DrawWindowContents() override;
};

