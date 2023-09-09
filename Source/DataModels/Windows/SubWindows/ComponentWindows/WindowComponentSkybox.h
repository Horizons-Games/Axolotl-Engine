#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentSkybox;
class WindowSkyboxInput;

class WindowComponentSkybox : public ComponentWindow
{
public:
	WindowComponentSkybox(ComponentSkybox* component);
	~WindowComponentSkybox() override;

protected:
	void DrawWindowContents() override;

private:
	std::unique_ptr <WindowSkyboxInput> skyboxInput;
};
