#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"
class ComponentBreakable;
class WindowComponentBreakable : public ComponentWindow
{
public:
	WindowComponentBreakable(ComponentBreakable* component);
	~WindowComponentBreakable() override;

protected:
	void DrawWindowContents() override;
};

