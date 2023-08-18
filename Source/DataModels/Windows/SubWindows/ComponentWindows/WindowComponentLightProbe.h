#pragma once
#include "ComponentWindow.h"

class ComponentLightProbe;

class WindowComponentLightProbe : public ComponentWindow
{
public:
	WindowComponentLightProbe(ComponentLightProbe* component);
	~WindowComponentLightProbe() override;

protected:
	void DrawWindowContents() override;
};

