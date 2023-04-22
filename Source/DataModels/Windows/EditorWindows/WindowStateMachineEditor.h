#pragma once

#include "EditorWindow.h"

class WindowResourceInput;
class Resource;
class ResourceStateMachine;
class EditorResourceInterface;

class WindowStateMachineEditor : public EditorWindow
{
public:
	WindowStateMachineEditor();
	~WindowStateMachineEditor() = default;

	void SetStateMachine(const std::weak_ptr<ResourceStateMachine>& resource);

	void SetResourceOnState(const std::shared_ptr<Resource>& resource);

protected:
	void DrawWindowContents() override;

private:
	std::shared_ptr<ResourceStateMachine> stateMachine;
	int stateIdSelected;
	unsigned long long transitionIdSelected;
	std::string parameterIdSelected;
	bool creatingTransition;
	bool openContextMenu;
	ImVec2 sizeState;

	std::unique_ptr<WindowResourceInput> inputResource;
};

inline void WindowStateMachineEditor::SetStateMachine(const std::weak_ptr<ResourceStateMachine>& resource)
{
	//stateMachine = resource;
}
