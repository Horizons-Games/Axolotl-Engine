#pragma once

#include "Windows/Window.h"

#include <list>
#include <vector>

#include "EditorWindows/EditorWindow.h"
#include "EditorWindows/WindowAbout.h"

class WindowMainMenu : public Window
{
public:
	WindowMainMenu(const std::vector<EditorWindow*>& editorWindows);
	~WindowMainMenu();

	static const std::string repositoryLink;

	void Draw(bool& enabled = defaultEnabled) override;

	inline bool IsWindowEnabled(int windowIndex) const
	{
		return windowsEnabled[windowIndex];
	}
	inline void SetWindowEnabled(int windowIndex, bool enabled)
	{
		windowsEnabled[windowIndex] = enabled;
	}

private:
	void DrawWindowsMenu();
	void DrawAbout();
	void DrawGithubLink() const;
	void DrawExit() const;

	static bool defaultEnabled;

	WindowAbout* about = nullptr;
	bool showAbout = false;

	int nWindows;
	std::vector<std::string> windowNames;
	std::vector<bool> windowsEnabled;
};

