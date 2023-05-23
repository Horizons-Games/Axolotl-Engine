#include "WindowMainMenu.h"
#include "Application.h"
#include "Application.h"
#include "Auxiliar/GameBuilder.h"
#include "Auxiliar/Utils/ConvertU8String.h"
#include "DataModels/Scene/Scene.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "ModuleScene.h"

#include "SDL.h"

#include "EditorWindows/ImporterWindows/WindowImportScene.h"
#include "EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "EditorWindows/ImporterWindows/WindowSaveScene.h"
#include "EditorWindows/WindowAbout.h"

const std::string WindowMainMenu::repositoryLink = "https://github.com/Horizons-Games/Axolotl-Engine";
bool WindowMainMenu::defaultEnabled = true;

WindowMainMenu::WindowMainMenu(Json& json) :
	Window("Main Menu"),
	showAbout(false),
	openPopup(false),
	isSaving(false),
	action(Actions::NONE),
	about(std::make_unique<WindowAbout>()),
	loadScene(std::make_unique<WindowLoadScene>()),
	saveScene(std::make_unique<WindowSaveScene>()),
	importScene(std::make_unique<WindowImportScene>())
{
	about = std::make_unique<WindowAbout>();

	for (const char* name : json.GetVectorNames())
	{
		std::pair<std::string, bool> windowNameAndEnabled =
			std::make_pair<std::string, bool>(std::string(name), bool(json[name]));
		windowNamesAndEnabled.push_back(windowNameAndEnabled);
	}
}

WindowMainMenu::~WindowMainMenu()
{
}

void WindowMainMenu::Draw(bool& enabled)
{
	if (openPopup)
		DrawPopup();
	else if (!isSaving && action != Actions::NONE)
	{
		if (action == Actions::NEW_SCENE)
		{
			CreateNewScene();
			action = Actions::NONE;
		}
		else if (action == Actions::EXIT)
			Exit();
	}
	if (isSaving)
		saveScene->SaveAsWindow(isSaving);
	if (ImGui::BeginMainMenuBar())
	{
		DrawFileMenu();
		DrawWindowMenu();
		DrawBuildGameMenu();
		DrawHelpMenu();
	}
	ImGui::EndMainMenuBar();
}

void WindowMainMenu::Exit()
{
	// to make it easier in terms of coupling between classes,
	// just push an SDL_QuitEvent to the event queue
	SDL_Event quitEvent;
	quitEvent.type = SDL_QUIT;
	SDL_PushEvent(&quitEvent);
}

void WindowMainMenu::CreateNewScene()
{
	std::unique_ptr<Scene> scene = std::make_unique<Scene>();
	scene->InitNewEmptyScene();
	App->GetModule<ModuleScene>()->SetLoadedScene(std::move(scene));
}

void WindowMainMenu::DrawPopup()
{
	ImGui::OpenPopup("Are you sure?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ModuleScene* scene = App->GetModule<ModuleScene>();

	if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Do you want to save the scene?\nAll your changes will be lost if you don't save them.");
		ImGui::Separator();
		std::string filePathName = scene->GetLoadedScene()->GetRoot()->GetName();
		if (ImGui::Button("Save scene", ImVec2(120, 0)))
		{
			if (filePathName != "New Scene")
				scene->SaveScene(filePathName + SCENE_EXTENSION);
			else
				isSaving = true;
			ImGui::CloseCurrentPopup();
			openPopup = false;
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Close without saving", ImVec2(240, 0)))
		{
			isSaving = false;
			openPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void WindowMainMenu::DrawFileMenu()
{
	ModuleScene* scene = App->GetModule<ModuleScene>();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::Button((ConvertU8String(ICON_IGFD_FILE) + " New Scene").c_str()))
		{
			openPopup = true;
			action = Actions::NEW_SCENE;
		}
		loadScene->DrawWindowContents();
		importScene->DrawWindowContents();
		if (ImGui::Button((ConvertU8String(ICON_IGFD_SAVE) + " Save Scene").c_str()))
		{
			std::string filePathName = scene->GetLoadedScene()->GetRoot()->GetName();
			// We should find a way to check if the scene has already been saved
			// Using "New Scene" is a patch
			if (filePathName != "New Scene")
				scene->SaveScene(filePathName + SCENE_EXTENSION);
			else
				isSaving = true;
		}
		saveScene->DrawWindowContents();
		if (ImGui::MenuItem("Exit"))
		{
			openPopup = true;
			action = Actions::EXIT;
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
			ShellExecuteA(NULL, "open", repositoryLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
		ImGui::EndMenu();
	}
	about->Draw(showAbout);
}

void WindowMainMenu::ShortcutSave()
{
	ModuleScene* scene = App->GetModule<ModuleScene>();
	std::string filePathName = scene->GetLoadedScene()->GetRoot()->GetName();

	if (filePathName != "New Scene")
	{
		scene->SaveScene(filePathName + SCENE_EXTENSION);
	}
	else
	{
		isSaving = true;
	}
}

void WindowMainMenu::DrawBuildGameMenu()
{
	if (ImGui::BeginMenu("Build"))
	{
		if (ImGui::MenuItem("Debug"))
		{
			builder::BuildGame(builder::BuildType::DEBUG_GAME);
		}
		if (ImGui::MenuItem("Release"))
		{
			builder::BuildGame(builder::BuildType::RELEASE_GAME);
		}
		ImGui::EndMenu();
	}
}
