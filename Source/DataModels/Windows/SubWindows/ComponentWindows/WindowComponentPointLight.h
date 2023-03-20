#pragma once

#include "ComponentWindow.h"

class ComponentPointLight;

class WindowComponentPointLight : public ComponentWindow
{
public:
	WindowComponentPointLight(ComponentPointLight* component);
	~WindowComponentPointLight() override;

protected:
	void DrawWindowContents() override;
};

