#pragma once

#include "ComponentWindow.h"

class ComponentObstacle;

class WindowComponentObstacle : public ComponentWindow
{
public:
	WindowComponentObstacle(ComponentObstacle* component);
	~WindowComponentObstacle() override;

protected:
	void DrawWindowContents() override;
};
