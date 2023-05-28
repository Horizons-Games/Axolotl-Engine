#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentCubemap;

class WindowComponentCubemap : public ComponentWindow
{
public:
	WindowComponentCubemap(ComponentCubemap* component);
	~WindowComponentCubemap() override;

protected:
	void DrawWindowContents() override;

private:
	constexpr static float max_intensity = 5000.0f;
};
