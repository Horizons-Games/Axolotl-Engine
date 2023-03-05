#include "WindowMainMenu.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"

#include "SDL.h"

const std::string WindowMainMenu::repositoryLink = "https://github.com/Horizons-Games/Axolotl-Engine";
bool WindowMainMenu::defaultEnabled = true;

WindowMainMenu::WindowMainMenu(const std::vector< std::unique_ptr<EditorWindow> >& editorWindows) :
	Window("Main Menu"), showAbout(false), about(std::make_unique<WindowAbout>()), 
	loadScene(std::make_unique<WindowLoadScene>()), saveScene(std::make_unique<WindowSaveScene>())
{

	for (const std::unique_ptr<EditorWindow>& window : editorWindows)
	{
		std::pair<std::string, bool> windowNameAndEnabled;
		if (window->GetName() == "Configuration") windowNameAndEnabled = std::make_pair(window->GetName(), false);
		else windowNameAndEnabled = std::make_pair(window->GetName(), defaultEnabled);
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
		DrawFileMenu();
		DrawWindowMenu();
		DrawHelpMenu();
	}
	ImGui::EndMainMenuBar();
}

void WindowMainMenu::DrawFileMenu()
{

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::Button(ICON_IGFD_FILE " New Scene"))
		{
			//std::unique_ptr<Scene> scene = std::make_unique<Scene>();
			//scene->InitNewEmptyScene();
			//App->scene->SetLoadedScene(std::move(scene));
		}
		loadScene->DrawWindowContents();
		if (ImGui::Button(ICON_IGFD_SAVE " Save Scene"))
		{
			std::string filePathName = fileDialogImporter.GetCurrentFileName();
			if (filePathName != "")	App->scene->SaveSceneToJson(filePathName);
			else saveScene->SaveAsWindow();
		}
		saveScene->DrawWindowContents();
		if (ImGui::MenuItem("Exit"))
		{
			//to make it easier in terms of coupling between classes,
			//just push an SDL_QuitEvent to the event queue
			SDL_Event quitEvent;
			quitEvent.type = SDL_QUIT;
			SDL_PushEvent(&quitEvent);
		}
		ImGui::EndMenu();
	}
}


void WindowMainMenu::DrawWindowMenu()
{
	if (ImGui::BeginMenu("Window"))
	{
		for (std::pair<std::string, bool>& windowNameAndEnabled : windowNamesAndEnabled)
		{
			ImGui::MenuItem(windowNameAndEnabled.first.c_str(), NULL, &windowNameAndEnabled.second);
		}
		ImGui::EndMenu();
	}
}

void WindowMainMenu::DrawHelpMenu()
{
	if (ImGui::BeginMenu("Help"))
	{
		ImGui::MenuItem("About Axolotl", NULL, &showAbout);

		if (ImGui::MenuItem("GitHub Link"))
		{
			ShellExecute(NULL, "open", repositoryLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::EndMenu();
	}
	about->Draw(showAbout);
}


