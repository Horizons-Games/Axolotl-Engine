#pragma once

#include "EditorWindow.h"

#include <memory>

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
	void DrawAddParameterMenu(std::shared_ptr<ResourceStateMachine>& stateAsShared);
	void DrawParameters(std::shared_ptr<ResourceStateMachine>& stateAsShared);
	void DrawStateEditor(std::shared_ptr<ResourceStateMachine>& stateAsShared);
	void DrawTransitionEditor(std::shared_ptr<ResourceStateMachine>& stateAsShared);

	void
		DrawGridStateMachine(const ImVec2& canvasP0, ImVec2& scrolling, const ImVec2& mouseDelta, ImDrawList* drawList);
	void DrawActualTransitionCreation(const ImVec2& posState,
									  const ImVec2& origin,
									  const ImVec2& mousePosInCanvas,
									  ImDrawList* drawList);

	void DrawTransitions(std::shared_ptr<ResourceStateMachine>& stateAsShared,
						 const ImVec2& origin,
						 ImDrawList* drawList);
	void DrawStates(std::shared_ptr<ResourceStateMachine>& stateAsShared,
					const ImVec2& origin,
					const ImVec2& mouseDelta,
					ImDrawList* drawList);

	void DrawRightClickPopUp(std::shared_ptr<ResourceStateMachine>& stateAsShared, const ImVec2& mousePosInCanvas);

	std::weak_ptr<ResourceStateMachine> stateMachine;
	int stateIdSelected;
	long long transitionIdSelected;
	bool creatingTransition;
	bool openContextMenu;
	ImVec2 sizeState;

	std::unique_ptr<WindowResourceInput> inputResource;
};

inline void WindowStateMachineEditor::SetStateMachine(const std::weak_ptr<ResourceStateMachine>& resource)
{
	stateMachine = resource;
	transitionIdSelected = -1;
	stateIdSelected = -1;
	creatingTransition = false;
	openContextMenu = false;
}
