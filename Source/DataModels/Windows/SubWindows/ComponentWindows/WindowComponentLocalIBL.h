#pragma once
#include "WindowComponentLight.h"

class ComponentLocalIBL;

class WindowComponentLocalIBL : public WindowComponentLight
{
public:
	WindowComponentLocalIBL(ComponentLocalIBL* component);
	~WindowComponentLocalIBL() override;

protected:
	void DrawWindowContents() override;
};

