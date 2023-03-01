#pragma once

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentMeshRenderer;

class WindowComponentBoundingBoxes : public ComponentWindow
{
public:
	WindowComponentBoundingBoxes(ComponentMeshRenderer* component);
	~WindowComponentBoundingBoxes() override;

protected:
	void DrawWindowContents() override;
};

