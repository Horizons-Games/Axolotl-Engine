#include "StdAfx.h"

#include "WindowAssetFolder.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "WindowFileBrowser.h"

WindowAssetFolder::WindowAssetFolder() :
	EditorWindow("File Browser"),
	browser(std::make_unique<WindowFileBrowser>()),
	type(ResourceType::Unknown),
	name("New")
{
}

WindowAssetFolder::~WindowAssetFolder()
{
}

void WindowAssetFolder::DrawWindowContents()
{
	if (ImGui::Button("Create Material"))
	{
		ImGui::OpenPopup("Select Name");
		type = ResourceType::Material;
		name = "NewMaterial";
	}

	ImGui::SameLine(0.0f, -1.0f);

	if (ImGui::Button("Create StateMachine"))
	{
		ImGui::OpenPopup("Select Name");
		type = ResourceType::StateMachine;
		name = "NewStateMachine";
	}
	ImGui::SameLine(0.0f, -1.0f);

	if (ImGui::Button("Create ParticleSystem"))
	{
		ImGui::OpenPopup("Select Name");
		type = ResourceType::ParticleSystem;
		name = "NewParticleSystem";
	}

	if (ImGui::BeginPopupModal("Select Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Name", &name[0], 64);
		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			App->GetModule<ModuleResources>()->CreateDefaultResource(type, name.c_str());
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine(0.0f, -1.0f);
	browser->DrawWindowContents();
}
