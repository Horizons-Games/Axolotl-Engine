#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentAnimation;

class WindowStateMachineInput :
	public WindowFileBrowser
{
public:
	WindowStateMachineInput(ComponentAnimation* state);
	~WindowStateMachineInput() override;

	void DoThisIfOk() override;

private:
	ComponentAnimation* component;
};
