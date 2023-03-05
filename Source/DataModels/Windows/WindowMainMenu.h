#pragma once

#include "EditorWindows/WindowAbout.h"
#include "EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "EditorWindows/ImporterWindows/WindowSaveScene.h"

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(const std::vector<std::unique_ptr<EditorWindow> >& editorWindows);
	~WindowMainMenu() override;

	static const std::string repositoryLink;

	void Draw(bool& enabled = defaultEnabled) override;

	bool IsWindowEnabled(int windowIndex) const;
	void SetWindowEnabled(int windowIndex, bool enabled);

private:
	void DrawWindowMenu();
	void DrawFileMenu();
	void DrawHelpMenu();

	static bool defaultEnabled;

	std::unique_ptr<WindowLoadScene> loadScene;
	std::unique_ptr<WindowSaveScene> saveScene;
	std::unique_ptr<WindowAbout> about;

	bool showAbout;

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