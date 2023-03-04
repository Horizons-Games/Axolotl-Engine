#pragma once

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentAmbient;

class WindowComponentAmbient : public ComponentWindow
{
public:
	WindowComponentAmbient(ComponentAmbient* component);
	~WindowComponentAmbient() override;

protected:
	void DrawWindowContents() override;
};