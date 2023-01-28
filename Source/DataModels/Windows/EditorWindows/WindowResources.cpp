#include "WindowResources.h"

#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

void WindowResources::DrawWindowContents()
{
	std::vector<UID> resourcesToDelete;

	if (ImGui::BeginTable("Resources", 2))
	{
		ImGui::TableSetupColumn("Unloaded", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();

		//in theory, since mapEntry is a reference to the one in the resources map,
		//it should not increase reference count while iterating since it's not a new pointer
		for (std::pair<const UID, std::shared_ptr<Resource> >& mapEntry : App->resources->resources)
		{
			DrawResource(mapEntry.second, resourcesToDelete);
		}

		ImGui::EndTable();
	}

	for (UID uidToDelete : resourcesToDelete)
	{
		App->resources->DeleteResource(uidToDelete);
	}
}

void WindowResources::DrawResource(const std::weak_ptr<Resource>& resource, std::vector<UID>& resourcesToDelete)
{
	long referenceCountBeforeLock = resource.use_count();
	std::shared_ptr<Resource> asShared = resource.lock();
	if (asShared)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(asShared->IsLoaded());
		std::string uidString = std::to_string(asShared->GetUID());

		ImGui::BeginGroup();

		ImGui::TextUnformatted(("UID: " + uidString).c_str());
		ImGui::TextUnformatted(("Name: " + asShared->GetFileName()).c_str());
		ImGui::TextUnformatted(("AssetPath: " + asShared->GetAssetsPath()).c_str());
		ImGui::TextUnformatted(("LibPath: " + asShared->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str());
		ImGui::TextUnformatted(("MetaPath: " + asShared->GetLibraryPath() + META_EXTENSION).c_str());
		//a bit yucky I guess
		ImGui::TextUnformatted(("Resource type: " + App->resources->GetNameOfType(asShared->GetType())).c_str());
		ImGui::TextUnformatted(("Reference count (should be 1): " + std::to_string(referenceCountBeforeLock)).c_str());

		ImGui::Separator();

		ImGui::EndGroup();

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(("Manage resource##" + uidString).c_str());
		}

		if (ImGui::BeginPopup(("Manage resource##" + uidString).c_str()))
		{
			if (!asShared->IsLoaded() && ImGui::MenuItem(("Load resource##" + uidString).c_str()))
			{
				asShared->Load();
			}
			if (asShared->IsLoaded() && ImGui::MenuItem(("Unload resource##" + uidString).c_str()))
			{
				asShared->Unload();
			}
			if (ImGui::MenuItem(("Delete resource##" + uidString).c_str()))
			{
				resourcesToDelete.push_back(asShared->GetUID());
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			//TODO: resource options
		}
	}
}
