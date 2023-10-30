#pragma once
#include "WindowComponentLight.h"

class ComponentPlanarReflection;

class WindowComponentPlanarReflection : public WindowComponentLight
{
public:
	WindowComponentPlanarReflection(ComponentPlanarReflection* component);
	~WindowComponentPlanarReflection() override;

protected:
	void DrawWindowContents() override;
};