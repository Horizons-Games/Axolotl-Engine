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

private:
	void ChangeScript(ComponentScript* newScript, const std::string selectedScript);
};

