#pragma once
#include "Module.h"

class EditorWindow;
class WindowMainMenu;
class WindowConsole;
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

	bool IsSceneFocused() const;

private:
	std::vector<std::string> lines;
	std::vector<EditorWindow*> windows;
	WindowMainMenu* mainMenu = nullptr;
	WindowConsole* console = nullptr;
	WindowScene* scene = nullptr;

	bool windowResized = false;
};

