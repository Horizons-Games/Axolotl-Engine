#include "WindowMainMenu.h"

#include "imgui.h"
#include "SDL.h"

const std::string WindowMainMenu::repositoryLink = "https://github.com/Pre-SuperAwesomeEngine/Engine";
bool WindowMainMenu::defaultEnabled = true;

WindowMainMenu::WindowMainMenu(const std::vector< std::unique_ptr<EditorWindow> >& editorWindows) : Window("Main Menu")
{
	about = std::make_unique<WindowAbout>();
	
	for (const std::unique_ptr<EditorWindow>& window : editorWindows)
	{
		std::pair<std::string, bool> windowNameAndEnabled = std::make_pair(window->GetName(), true);
		windowNamesAndEnabled.push_back(windowNameAndEnabled);
	}
}

WindowMainMenu::~WindowMainMenu()
{
}

void WindowMainMenu::Draw(bool& enabled)
{
	if (ImGui::BeginMainMenuBar())
	{
		DrawWindowsMenu();
		DrawAbout();
		DrawGithubLink();
		DrawExit();
		
	}
	ImGui::EndMainMenuBar();
}

void WindowMainMenu::DrawWindowsMenu()
{
	if (ImGui::BeginMenu("Windows"))
	{
		for (std::pair<std::string, bool>& windowNameAndEnabled : windowNamesAndEnabled)
		{
			ImGui::MenuItem(windowNameAndEnabled.first.c_str(), NULL, &windowNameAndEnabled.second);
		}
		ImGui::EndMenu();
	}
}

void WindowMainMenu::DrawAbout()
{
	if (ImGui::MenuItem("About"))
	{
		showAbout = !showAbout;
	}		
	about->Draw(showAbout);
}

void WindowMainMenu::DrawGithubLink() const
{
	if (ImGui::MenuItem("GitHub Link"))
	{
		ShellExecute(NULL, "open", repositoryLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

void WindowMainMenu::DrawExit() const
{
	if (ImGui::MenuItem("Exit"))
	{
		//to make it easier in terms of coupling between classes,
		//just push an SDL_QuitEvent to the event queue
		SDL_Event quitEvent;
		quitEvent.type = SDL_QUIT;
		SDL_PushEvent(&quitEvent);
	}
}
