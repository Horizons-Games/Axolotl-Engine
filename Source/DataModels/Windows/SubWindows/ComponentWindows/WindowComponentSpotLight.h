#pragma once

#include "ComponentWindow.h"

class ComponentSpotLight;

class WindowComponentSpotLight : public ComponentWindow
{
public:
	WindowComponentSpotLight(ComponentSpotLight* component);
	~WindowComponentSpotLight() override = default;

protected:
	void DrawWindowContents() override;
};

