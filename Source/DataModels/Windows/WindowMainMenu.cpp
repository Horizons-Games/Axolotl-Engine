#include "WindowMainMenu.h"

#include "SDL.h"

const std::string WindowMainMenu::repositoryLink = "https://github.com/Horizons-Games/Axolotl-Engine";
bool WindowMainMenu::defaultEnabled = true;

WindowMainMenu::WindowMainMenu(const std::vector< std::unique_ptr<EditorWindow> >& editorWindows) : 
	Window("Main Menu"), showAbout(false)
{
	
	enabled = defaultEnabled;
	about = std::make_unique<WindowAbout>();
	
	for (const std::unique_ptr<EditorWindow>& window : editorWindows)
	{
		std::pair<std::string, std::reference_wrapper<bool>> windowNameAndEnabled = { window->GetName(), window->enabled };
		windowNamesAndEnabled.push_back(windowNameAndEnabled);		
	}
}

WindowMainMenu::~WindowMainMenu()
{
}

void WindowMainMenu::Draw()
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
			ImGui::MenuItem(windowNameAndEnabled.first.c_str(), NULL, &windowNameAndEnabled.second.get());
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
	about->Draw();
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
