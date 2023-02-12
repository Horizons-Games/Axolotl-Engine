#pragma once

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentBoundingBoxes;

class WindowComponentBoundingBoxes : public ComponentWindow
{
public:
	WindowComponentBoundingBoxes(const std::weak_ptr<ComponentBoundingBoxes>& component);
	~WindowComponentBoundingBoxes() override = default;

protected:
	void DrawWindowContents() override;
};

