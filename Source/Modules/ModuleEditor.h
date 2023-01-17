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

	bool IsSceneFocused() const;

	unsigned dock_space_id = 0;
	unsigned dock_main_id = 0;
	unsigned dock_left_id = 0;
	unsigned dock_right_id = 0;
	unsigned dock_down_id = 0;
	unsigned dock_center_id = 0;

	unsigned dock_spaceLeft_id = 0;
	unsigned dock_spaceRight_id = 0;
	unsigned dock_spaceDown_id = 0;

private:
	std::vector<std::string> lines;
	std::vector<std::shared_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu = nullptr;
	std::shared_ptr<WindowScene> scene = nullptr;
	bool firstDeployed = false;

	bool windowResized = false;
};

