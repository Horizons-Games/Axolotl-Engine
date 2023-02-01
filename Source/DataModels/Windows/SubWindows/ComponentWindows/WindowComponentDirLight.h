#pragma once

#include "ComponentWindow.h"

class ComponentDirLight;

class WindowComponentDirLight : public ComponentWindow
{
public:
	WindowComponentDirLight(const std::weak_ptr<ComponentDirLight>& component);
	~WindowComponentDirLight() override = default;

protected:
	void DrawWindowContents() override;
};

