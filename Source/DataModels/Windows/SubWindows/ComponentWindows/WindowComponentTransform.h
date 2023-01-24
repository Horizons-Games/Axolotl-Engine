#pragma once

#include "ComponentWindow.h"

class ComponentTransform;

class WindowComponentTransform : public ComponentWindow
{
public:
	WindowComponentTransform(const std::weak_ptr<ComponentTransform>& component);
	~WindowComponentTransform() override = default;

protected:
	void DrawWindowContents() override;

private:
	void DrawTransformTable();
};

