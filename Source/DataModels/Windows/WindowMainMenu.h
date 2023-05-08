#pragma once

#include "EditorWindows/WindowAbout.h"
#include "EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "EditorWindows/ImporterWindows/WindowSaveScene.h"

class Json;

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(Json &json);
	~WindowMainMenu() override;	
	static const std::string repositoryLink;

	void Draw(bool &enabled=defaultEnabled) override;

	bool IsWindowEnabled(int windowIndex) const;
	void SetWindowEnabled(int windowIndex, bool enabled);
	void ShortcutSave();

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
	std::unique_ptr<WindowAbout> about;

	bool showAbout;
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

	std::vector<std::pair<std::string, bool> > windowNamesAndEnabled;
	
};

inline bool WindowMainMenu::IsWindowEnabled(int windowIndex) const
{
	return windowNamesAndEnabled[windowIndex].second;	
}

inline void WindowMainMenu::SetWindowEnabled(int windowIndex, bool enabled)
{
	windowNamesAndEnabled[windowIndex].second = enabled;	
}