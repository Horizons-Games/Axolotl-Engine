#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentCubeMap;

class WindowComponentCubeMap : public ComponentWindow
{
public:
	WindowComponentCubeMap(ComponentCubeMap* component);
	~WindowComponentCubeMap() override;

protected:
	void DrawWindowContents() override;
};
