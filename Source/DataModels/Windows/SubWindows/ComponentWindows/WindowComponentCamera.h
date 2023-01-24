#pragma once

#include "ComponentWindow.h"

class ComponentCamera;

class WindowComponentCamera : public ComponentWindow
{
public:
	WindowComponentCamera(const std::weak_ptr<ComponentCamera>& component);
	~WindowComponentCamera() override = default;

protected:
	void DrawWindowContents() override;
};

