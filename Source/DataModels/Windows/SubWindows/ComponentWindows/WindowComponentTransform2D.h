#pragma once
#include "ComponentWindow.h"

class ComponentTransform2D;

class WindowComponentTransform2D :
	public ComponentWindow
{
public:
	WindowComponentTransform2D(ComponentTransform2D* component);
	~WindowComponentTransform2D() override;

protected:
	void DrawTransformTable();
	void DrawWindowContents() override;

};
