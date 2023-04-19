#pragma once

#include "ComponentWindow.h"

#include "Math/float3.h"

class ComponentScript;

class WindowComponentScript : public ComponentWindow
{
public:
	WindowComponentScript(ComponentScript* component);
	~WindowComponentScript() override;

protected:
	void DrawWindowContents() override;
	int current_item = 0;
};

