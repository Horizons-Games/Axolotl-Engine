#pragma once
#include "Module.h"

class EditorWindow;
class WindowMainMenu;
class WindowDebug;
class WindowScene;
class WindowInspector;
class WindowStateMachineEditor;
class Resource;
class ResourceStateMachine;
class GameObject;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor() override;

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void Resized();

	void SetStateMachineWindowEditor(const std::weak_ptr<ResourceStateMachine>& resource);
	void SetResourceOnStateMachineEditor(const std::shared_ptr<Resource>& resource);

	const WindowScene* GetScene() const;
	const WindowDebug* GetDebugOptions() const;

	bool IsSceneFocused() const;
	void SetResourceOnInspector(const std::weak_ptr<Resource>& resource) const;
	void RefreshInspector() const;
	std::pair<int, int> GetAvailableRegion();

private:
	void CopyAnObject();
	void PasteAnObject();
	void CutAnObject();
	void DuplicateAnObject();

	std::vector<std::unique_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu = nullptr;
	std::unique_ptr<WindowDebug> debugOptions = nullptr;
	std::unique_ptr<WindowStateMachineEditor> stateMachineEditor;
	bool stateMachineWindowEnable;

	WindowInspector* inspector;
	WindowScene* scene;
	bool windowResized;

	GameObject* copyObject;
};

inline const WindowScene* ModuleEditor::GetScene() const
{
	return scene;
}

inline const WindowDebug* ModuleEditor::GetDebugOptions() const
{
	return debugOptions.get();
}
