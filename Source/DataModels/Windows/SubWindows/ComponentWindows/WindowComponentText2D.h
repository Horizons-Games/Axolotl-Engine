#pragma once

#include "ComponentWindow.h"

class ComponentText2D;
class WindowFontInput;

class WindowComponentText2D : public ComponentWindow
{
public:
	WindowComponentText2D(ComponentText2D * component);
	~WindowComponentText2D() override;

protected:
	void DrawWindowContents() override;	
	std::unique_ptr<WindowFontInput> inputFont;
	ComponentText2D* componentText2D;
	char str1[128];		
};

