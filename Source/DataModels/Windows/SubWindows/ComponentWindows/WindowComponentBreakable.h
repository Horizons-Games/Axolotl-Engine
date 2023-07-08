#pragma once
#include "ComponentWindow.h"
class ComponentBreakable;
class WindowComponentBreakable : public ComponentWindow
{
public:
	WindowComponentBreakable(ComponentBreakable* component);
	~WindowComponentBreakable() override;

protected:
	void DrawWindowContents() override;
};

