#pragma once

#include "EditorWindow.h"

class ResourceStateMachine;
class EditorResourceInterface;

class WindowStateMachineEditor : public EditorWindow
{
public:
	WindowStateMachineEditor();
	~WindowStateMachineEditor() = default;

	void SetStateMachine(const std::weak_ptr<ResourceStateMachine>& resource);

protected:
	void DrawWindowContents() override;

private:
	std::shared_ptr<ResourceStateMachine> stateMachine;
	int stateIdSelected;
	bool creatingTransition;
	int transitionIdSelected;
	bool openContextMenu;
};

inline void WindowStateMachineEditor::SetStateMachine(const std::weak_ptr<ResourceStateMachine>& resource)
{
	//stateMachine = resource;
}
