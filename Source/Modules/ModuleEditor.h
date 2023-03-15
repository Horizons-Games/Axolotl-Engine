#pragma once
#include "Module.h"

class EditorWindow;
class WindowMainMenu;
class WindowScene;

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

	const WindowScene* GetScene() const;
	const WindowMainMenu* GetMainMenu() const;

	bool IsSceneFocused() const;

private:
	std::vector<std::unique_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu;
	WindowScene* scene;

	bool windowResized;
};

inline const WindowScene* ModuleEditor::GetScene() const
{
	return scene;
}

inline const WindowMainMenu* ModuleEditor::GetMainMenu() const
{
	return mainMenu.get();
}