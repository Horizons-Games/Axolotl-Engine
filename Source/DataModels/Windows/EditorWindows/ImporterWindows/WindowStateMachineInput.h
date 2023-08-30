#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class StateMachine;

class WindowStateMachineInput : public WindowFileBrowser
{
public:
	WindowStateMachineInput(StateMachine* stateMachineInstance);
	~WindowStateMachineInput() override;

	void SetStateMachine(StateMachine* statemachine);

	void DoThisIfOk() override;

private:
	StateMachine* stateMachineInstance;
};

inline void WindowStateMachineInput::SetStateMachine(StateMachine* statemachine)
{
	stateMachineInstance = statemachine;
}
