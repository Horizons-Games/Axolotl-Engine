#pragma once

#include "ComponentWindow.h"

class ComponentPointLight;

class WindowComponentPointLight : public ComponentWindow
{
public:
	WindowComponentPointLight(ComponentPointLight* component);
	~WindowComponentPointLight() override = default;

protected:
	void DrawWindowContents() override;
};

