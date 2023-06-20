#pragma once
#include "ComponentWindow.h"

class ComponentSlider;

class WindowComponentSlider : public ComponentWindow
{
public:
	WindowComponentSlider(ComponentSlider* component);
	~WindowComponentSlider() override;

protected:
	void DrawWindowContents() override;
};
