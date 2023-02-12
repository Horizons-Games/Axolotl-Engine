#pragma once

#include "DataModels/Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentAmbient;

class WindowComponentAmbient : public ComponentWindow
{
public:
	WindowComponentAmbient(const std::weak_ptr<ComponentAmbient>& component);
	~WindowComponentAmbient() override = default;

protected:
	void DrawWindowContents() override;
};