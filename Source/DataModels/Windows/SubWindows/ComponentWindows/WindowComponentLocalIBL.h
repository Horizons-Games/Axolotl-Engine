#pragma once
#include "ComponentWindow.h"

class ComponentLocalIBL;

class WindowComponentLocalIBL : public ComponentWindow
{
public:
	WindowComponentLocalIBL(ComponentLocalIBL* component);
	~WindowComponentLocalIBL() override;

protected:
	void DrawWindowContents() override;
};

