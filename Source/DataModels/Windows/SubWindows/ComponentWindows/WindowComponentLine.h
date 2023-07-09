#pragma once
#include "ComponentWindow.h"

class ComponentLine;
class ImGradient;

class WindowComponentLine : public ComponentWindow
{
public:
	WindowComponentLine(ComponentLine* component);
	~WindowComponentLine() override;

protected:
	void DrawWindowContents() override;
};
