#pragma once

#include "ComponentWindow.h"

class ComponentLight;

class WindowComponentLight : public ComponentWindow
{
public:
	WindowComponentLight(const std::weak_ptr<ComponentLight>& component);
	~WindowComponentLight() override = default;

protected:
	void DrawWindowContents() override;
};

