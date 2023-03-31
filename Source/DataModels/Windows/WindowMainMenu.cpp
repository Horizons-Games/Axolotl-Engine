#include "WindowMainMenu.h"
#include"Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"
#include "SDL.h"

const std::string WindowMainMenu::repositoryLink = "https://github.com/Horizons-Games/Axolotl-Engine";
bool WindowMainMenu::defaultEnabled = true;

WindowMainMenu::WindowMainMenu(Json &json ) :
	Window("Main Menu"), showAbout(false)
{	
	enabled = defaultEnabled;
	about = std::make_unique<WindowAbout>();
	std::vector<const char*> names;			
	json.getVectorNames(names);
	
	for (const char* name:names)
	{
		std::pair<std::string, bool> windowNameAndEnabled = std::make_pair<std::string, bool>(std::string(name), bool(json[name]));
		windowNamesAndEnabled.push_back(windowNameAndEnabled);		
	}
}

WindowMainMenu::~WindowMainMenu()
{
}

void WindowMainMenu::Draw(bool &enabled)
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
		for (auto& windowNameAndEnabled : windowNamesAndEnabled)
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
		enabled = !showAbout;
	}		
	about->Draw(enabled);
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

