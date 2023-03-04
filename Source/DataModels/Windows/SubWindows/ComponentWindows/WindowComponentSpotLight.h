#pragma once

#include "ComponentWindow.h"

class ComponentSpotLight;

class WindowComponentSpotLight : public ComponentWindow
{
public:
	WindowComponentSpotLight(ComponentSpotLight* component);
	~WindowComponentSpotLight() override;

protected:
	void DrawWindowContents() override;
};

