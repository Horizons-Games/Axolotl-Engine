#pragma once

#include "Windows/Window.h"

#include "EditorWindows/EditorWindow.h"
#include "EditorWindows/WindowAbout.h"

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(const std::vector<std::unique_ptr<EditorWindow> >& editorWindows);
	~WindowMainMenu();

	static const std::string repositoryLink;

	void Draw(bool& enabled = defaultEnabled) override;

	bool IsWindowEnabled(int windowIndex) const;
	void SetWindowEnabled(int windowIndex, bool enabled);

private:
	void DrawWindowsMenu();
	void DrawAbout();
	
	void DrawGithubLink() const;
	void DrawExit() const;

	static bool defaultEnabled;

	std::unique_ptr<WindowAbout> about;
	
	bool showAbout;
	
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
