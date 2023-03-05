#pragma once

#include "ComponentWindow.h"

class ComponentLight;

class WindowComponentLight : public ComponentWindow
{
public:
	WindowComponentLight(ComponentLight* component);
	WindowComponentLight(const std::string& name, ComponentLight* component);
	~WindowComponentLight() override;

protected:
	void DrawWindowContents() override;

	const static float max_intensity;
};

