#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentSkybox;

class WindowComponentSkybox : public ComponentWindow
{
public:
	WindowComponentSkybox(ComponentSkybox* component);
	~WindowComponentSkybox() override;

protected:
	void DrawWindowContents() override;
};
