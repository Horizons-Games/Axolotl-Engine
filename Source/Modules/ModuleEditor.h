#pragma once
#include "Module.h"
#include "GameObject/GameObject.h"
#include "imgui.h"

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

	GameObject* Selected_Object = nullptr;

	void Resized();

	bool IsSceneFocused() const;

	ImVec4 title_color = ImVec4(0.35f, 0.69f, 0.87f, 1.0f);
	ImVec4 text_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

private:
	std::vector<std::string> lines;
	std::vector<std::shared_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu = nullptr;
	std::shared_ptr<WindowScene> scene = nullptr;

	bool windowResized = false;
};

