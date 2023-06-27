#pragma once

#include "ComponentWindow.h"

class ComponentCameraSample;

class WindowComponentCameraSample : public ComponentWindow
{
public:
	WindowComponentCameraSample(ComponentCameraSample* component);
	~WindowComponentCameraSample() override;

protected:
	void DrawWindowContents() override;
};
