#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentRender;

class WindowComponentRender : public ComponentWindow
{
public:
	WindowComponentRender(ComponentRender* component);
	~WindowComponentRender() override;

protected:
	void DrawWindowContents() override;

private:
	void Init();
};
