#pragma once

#include "ComponentWindow.h"

class ComponentCamera;

class WindowComponentCamera : public ComponentWindow
{
public:
	WindowComponentCamera(ComponentCamera* component);
	~WindowComponentCamera() override = default;

protected:
	void DrawWindowContents() override;
};

