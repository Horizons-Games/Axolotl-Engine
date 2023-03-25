#pragma once
#include "ComponentWindow.h"

class ComponentImage;

class WindowComponentImage :
	public ComponentWindow
{
public:
	WindowComponentImage(ComponentImage* component);
	~WindowComponentImage() override;

protected:
	void DrawWindowContents() override;
};

