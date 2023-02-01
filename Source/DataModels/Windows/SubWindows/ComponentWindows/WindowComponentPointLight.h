#pragma once

#include "ComponentWindow.h"

class ComponentPointLight;

class WindowComponentPointLight : public ComponentWindow
{
public:
	WindowComponentPointLight(const std::weak_ptr<ComponentPointLight>& component);
	~WindowComponentPointLight() override = default;

protected:
	void DrawWindowContents() override;
};

