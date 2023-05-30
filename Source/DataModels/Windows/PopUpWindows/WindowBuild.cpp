#include "DataModels/Windows/PopUpWindows/WindowBuild.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

#include "Auxiliar/GameBuilder.h"

#include <map>

namespace
{
const std::map<std::string, builder::BuildType> buildTypes = { { "Debug", builder::BuildType::DEBUG_GAME },
															   { "Release", builder::BuildType::RELEASE_GAME } };
}

WindowBuild::WindowBuild() : PopUpWindow("Build Game")
{
}

WindowBuild::~WindowBuild()
{
}

void WindowBuild::DrawWindowContents()
{
	DrawSceneComboBox();
	DrawBuildTypeComboBox();
	DrawGenerateZipCheckmark();
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

void WindowBuild::DrawGenerateZipCheckmark()
{
	ImGui::Checkbox("Generate a new assets zip file", &generateZip);
}

void WindowBuild::DrawBuildButton()
{
	bool formNotFilled = selectedScene == "-" || selectedBuild == "-";
	if (formNotFilled)
	{
		ImGui::BeginDisabled();
	}
	// for now ignore everything and keep current flow
	if (ImGui::Button("Build game"))
	{
		startedBuild = true;
		builder::BuildGame(buildTypes.find(selectedBuild)->second);
	}
	if (formNotFilled)
	{
		ImGui::EndDisabled();
	}
}
