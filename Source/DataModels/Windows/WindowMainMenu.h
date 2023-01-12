#pragma once

#include "Windows/Window.h"

#include <list>
#include <vector>
#include <memory>

#include "EditorWindows/EditorWindow.h"
#include "EditorWindows/WindowAbout.h"
#include "EditorWindows/WindowImporter.h"
#include "EditorWindows/WindowFileBrowser.h"

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(const std::vector<std::shared_ptr<EditorWindow> >& editorWindows);
	~WindowMainMenu();

	static const std::string repositoryLink;

	void Draw(bool& enabled = defaultEnabled) override;

	bool IsWindowEnabled(int windowIndex) const;
	void SetWindowEnabled(int windowIndex, bool enabled);

private:
	void DrawWindowsMenu();
	void DrawAbout();
	void DrawFileDialog();
	void DrawFileBrowser();
	void DrawGithubLink() const;
	void DrawExit() const;

	static bool defaultEnabled;

	std::unique_ptr<WindowAbout> about;
	std::unique_ptr<WindowImporter> importer;
	std::unique_ptr<WindowFileBrowser> browser;
	bool showAbout = false;
	bool showFileDialog = false;
	bool showFileBrowser = false;

	int nWindows;
	std::vector<std::string> windowNames;
	std::vector<bool> windowsEnabled;
};

inline bool WindowMainMenu::IsWindowEnabled(int windowIndex) const
{
	return windowsEnabled[windowIndex];
}

inline void WindowMainMenu::SetWindowEnabled(int windowIndex, bool enabled)
{
	windowsEnabled[windowIndex] = enabled;
}
