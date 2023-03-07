#pragma once

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentBoundingBoxes;

class WindowComponentBoundingBoxes : public ComponentWindow
{
public:
	WindowComponentBoundingBoxes(ComponentBoundingBoxes* component);
	~WindowComponentBoundingBoxes() override;

protected:
	void DrawWindowContents() override;
};

