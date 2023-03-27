#pragma once
#include "ComponentWindow.h"

class ComponentButton;

class WindowComponentButton :
	public ComponentWindow
{
public:
	WindowComponentButton(ComponentButton* component);
	~WindowComponentButton() override;

protected:
	void DrawWindowContents() override;

};

