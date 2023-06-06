#include "DataModels/Windows/PopUpWindows/WindowBuild.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Auxiliar/GameBuilder.h"

#include <map>

namespace
{
const std::string noneSelected = "-";
const std::map<std::string, builder::BuildType> buildTypes = { { "Debug", builder::BuildType::DEBUG_GAME },
															   { "Release", builder::BuildType::RELEASE_GAME } };
}

WindowBuild::WindowBuild() :
	PopUpWindow("Build Game"),
	selectedScene(noneSelected),
	selectedBuild(noneSelected),
	generateZip(true),
	startedBuild(false)
{
}

WindowBuild::~WindowBuild()
{
}

void WindowBuild::Reset()
{
	selectedScene = noneSelected;
	selectedBuild = noneSelected;
	generateZip = true;
	startedBuild = false;
}

void WindowBuild::DrawWindowContents()
{
	DrawSceneComboBox();
	DrawBuildTypeComboBox();
	DrawGenerateZipCheckbox();
	DrawBuildButton();
}

void WindowBuild::DrawSceneComboBox()
{
	if (ImGui::BeginCombo("Select a starting scene", selectedScene.c_str()))
	{
		std::vector<std::string> allScenes = App->GetModule<ModuleFileSystem>()->ListFiles(SCENE_PATH);
		for (const std::string& scene : allScenes)
		{
			bool isSelected = scene == selectedScene;
			if (ImGui::Selectable(scene.c_str(), isSelected))
			{
				selectedScene = scene;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void WindowBuild::DrawBuildTypeComboBox()
{
	if (ImGui::BeginCombo("Select a build configuration", selectedBuild.c_str()))
	{
		for (const auto& buildConfig : buildTypes)
		{
			bool isSelected = selectedBuild == buildConfig.first;
			if (ImGui::Selectable(buildConfig.first.c_str(), isSelected))
			{
				selectedBuild = buildConfig.first;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void WindowBuild::DrawGenerateZipCheckbox()
{
	ImGui::Checkbox("Generate a new assets zip file", &generateZip);
}

void WindowBuild::DrawBuildButton()
{
	bool formNotFilled = selectedScene == noneSelected || selectedBuild == noneSelected;
	if (formNotFilled)
	{
		ImGui::BeginDisabled();
	}
	// for now ignore everything and keep current flow
	if (ImGui::Button("Build game"))
	{
		startedBuild = true;
		builder::BuildGame(buildTypes.find(selectedBuild)->second, generateZip);
		CreateStartConfigFile();
	}
	if (formNotFilled)
	{
		ImGui::EndDisabled();
	}
}

void WindowBuild::CreateStartConfigFile()
{
	rapidjson::Document doc;
	Json startConfig(doc, doc);

	startConfig["StartingScene"] = selectedScene.c_str();

	rapidjson::StringBuffer buffer;
	startConfig.toBuffer(buffer);

	std::string path = GAME_STARTING_CONFIG;

	App->GetModule<ModuleFileSystem>()->Save(path.c_str(), buffer.GetString(), buffer.GetSize());
}
