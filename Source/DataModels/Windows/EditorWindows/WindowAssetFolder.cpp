#include "WindowAssetFolder.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "WindowFileBrowser.h"

WindowAssetFolder::WindowAssetFolder() : EditorWindow("File Browser"), browser(std::make_unique<WindowFileBrowser>())
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
	}
	if (ImGui::BeginPopupModal("Select Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char name[64] = "NewMaterial";
		ImGui::InputText("Name", &name[0], 64);
		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			App->GetModule<ModuleResources>()->CreateDefaultResource(ResourceType::Material, name);
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
