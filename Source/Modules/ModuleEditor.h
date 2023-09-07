#pragma once
#include "Module.h"

class EditorWindow;
class WindowMainMenu;
class WindowDebug;
class WindowScene;
class WindowInspector;
class WindowStateMachineEditor;
class WindowLoading;
class Resource;
class StateMachine;
class GameObject;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor() override;

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	void Resized();

	void SetStateMachineWindowEditor(StateMachine* resourceInstance,const std::string& instanceName);
	void SetResourceOnStateMachineEditor(const std::shared_ptr<Resource>& resource);

	const WindowScene* GetScene() const;
	WindowMainMenu* GetMainMenu() const;
	const WindowDebug* GetDebugOptions() const;

	bool IsSceneFocused() const;
	void SetResourceOnInspector(const std::weak_ptr<Resource>& resource) const;
	void RefreshInspector() const;
	std::pair<float, float> GetAvailableRegion();

	bool IsSceneFullscreen() const;
	void ToggleFullscreenScene();

	bool IsEditorControlEnabled() const;
	void ToggleEditorControl();

private:
	std::vector<std::unique_ptr<EditorWindow>> windows;
	std::unique_ptr<WindowMainMenu> mainMenu = nullptr;
	std::unique_ptr<WindowDebug> debugOptions = nullptr;
	std::unique_ptr<WindowStateMachineEditor> stateMachineEditor = nullptr;
	bool stateMachineWindowEnable;

	WindowInspector* inspector;
	WindowScene* scene;

	bool windowResized;
	bool fullscreenScene;
	bool editorControl;

	char* StateWindows();
	void CreateFolderSettings();
	static const std::string settingsFolder;
	static const std::string set;

	void DrawLoadingPopUp();
	std::unique_ptr<WindowLoading> loadingPopUp = nullptr;
};

inline void ModuleEditor::Resized()
{
	windowResized = true;
}

inline const WindowScene* ModuleEditor::GetScene() const
{
	return scene;
}

inline WindowMainMenu* ModuleEditor::GetMainMenu() const
{
	return mainMenu.get();
}

inline const WindowDebug* ModuleEditor::GetDebugOptions() const
{
	return debugOptions.get();
}

inline void ModuleEditor::ToggleFullscreenScene()
{
	fullscreenScene = !fullscreenScene;
}

inline void ModuleEditor::ToggleEditorControl()
{
	editorControl = !editorControl;
}

inline bool ModuleEditor::IsSceneFullscreen() const
{
	return fullscreenScene;
}

inline bool ModuleEditor::IsEditorControlEnabled() const
{
	return editorControl;
}
