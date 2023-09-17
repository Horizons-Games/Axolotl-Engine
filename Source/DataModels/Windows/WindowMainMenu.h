#pragma once

#include "Window.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

class WindowLoadScene;
class WindowSaveScene;
class WindowImportScene;
class WindowAbout;
class WindowBuild;

class Json;

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(Json& json);
	~WindowMainMenu() override;
	static const std::string repositoryLink;

	void Draw(bool& enabled = defaultEnabled) override;

	bool IsWindowEnabled(int windowIndex) const;
	void SetWindowEnabled(int windowIndex, bool enabled);
	void ShortcutSave();

	bool IsLoadingScene() const;

private:
	void DrawWindowMenu();
	void DrawFileMenu();
	void DrawHelpMenu();
	void DrawBuildGameMenu();
	void DrawPopup();
	void CreateNewScene();
	void Exit();

	static bool defaultEnabled;

	std::unique_ptr<WindowLoadScene> loadScene;
	std::unique_ptr<WindowSaveScene> saveScene;
	std::unique_ptr<WindowImportScene> importScene;
	std::unique_ptr<WindowAbout> about;
	std::unique_ptr<WindowBuild> build;

	bool showAbout;
	bool showBuild;
	bool openPopup;
	bool isSaving;

	enum class Actions
	{
		NONE,
		NEW_SCENE,
		EXIT
	};

	Actions action;

	ImGuiFileDialog fileDialogImporter;

	std::vector<std::pair<std::string, bool>> windowNamesAndEnabled;
};

inline bool WindowMainMenu::IsWindowEnabled(int windowIndex) const
{
	return windowNamesAndEnabled[windowIndex].second;
}

inline void WindowMainMenu::SetWindowEnabled(int windowIndex, bool enabled)
{
	windowNamesAndEnabled[windowIndex].second = enabled;
}