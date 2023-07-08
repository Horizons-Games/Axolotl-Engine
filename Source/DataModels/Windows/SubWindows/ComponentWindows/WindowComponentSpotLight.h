#pragma once

#include "WindowComponentLight.h"

class ComponentSpotLight;

class WindowComponentSpotLight : public WindowComponentLight
{
public:
	WindowComponentSpotLight(ComponentSpotLight* component);
	~WindowComponentSpotLight() override;

protected:
	void DrawWindowContents() override;
};
