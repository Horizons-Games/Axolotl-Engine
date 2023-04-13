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

	bool IsSceneFocused() const;

private:
	std::vector<std::unique_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu;
	WindowScene* scene;

	bool windowResized;
	char* StateWindows();
	void CreateFolderSettings();
	static const std::string settingsFolder;
};

inline const WindowScene* ModuleEditor::GetScene() const
{
	return scene;
}