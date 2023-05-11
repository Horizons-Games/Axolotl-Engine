#pragma once
#include "ComponentWindow.h"

class ComponentAnimation;
class WindowStateMachineInput;

class WindowComponentAnimation :
	public ComponentWindow
{
public:
	WindowComponentAnimation(ComponentAnimation* component);
	~WindowComponentAnimation() override;

protected:
	void DrawWindowContents() override;
private:
	std::unique_ptr<WindowStateMachineInput> inputState;
};

