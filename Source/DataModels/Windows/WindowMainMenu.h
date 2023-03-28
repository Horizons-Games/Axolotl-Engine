#pragma once

#include "EditorWindows/WindowAbout.h"
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
private:
	void DrawWindowsMenu();
	void DrawAbout();
	void DrawGithubLink() const;
	void DrawExit() const;
	

	static bool defaultEnabled;
	
	std::unique_ptr<WindowAbout> about;
	
	bool showAbout;
	
	/*std::vector<std::pair<std::string, std::reference_wrapper<bool>> > windowNamesAndEnabled;*/
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
