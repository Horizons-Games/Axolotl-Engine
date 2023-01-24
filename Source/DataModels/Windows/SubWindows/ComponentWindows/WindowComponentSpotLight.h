#pragma once

#include "ComponentWindow.h"

class ComponentSpotLight;

class WindowComponentSpotLight : public ComponentWindow
{
public:
	WindowComponentSpotLight(const std::weak_ptr<ComponentSpotLight>& component);
	~WindowComponentSpotLight() override = default;

protected:
	void DrawWindowContents() override;
};

