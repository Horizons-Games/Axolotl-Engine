#include "WindowInspectorResource.h"

#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceTexture.h"

WindowInspectorResource::WindowInspectorResource() : EditorWindow("InspectorResource")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspectorResource::~WindowInspectorResource()
{
}

void WindowInspectorResource::DrawWindowContents()
{
	UID aux = App->resources->ImportResource("Assets/Textures/front.jpg");
	std::shared_ptr<ResourceTexture> resource = App->resources->RequestResource<ResourceTexture>(aux).lock();
	resource->Load();
	//TODO When user select another resource Unload the last one

	ImGui::Text(resource->GetFileName().c_str());
	switch (resource->GetType())
	{
	case ResourceType::Texture:
		DrawTextureOptions();
		break;
	default:
		break;
	}
}

void WindowInspectorResource::DrawTextureOptions()
{
	UID aux = App->resources->ImportResource("Assets/Textures/front.jpg");
	std::shared_ptr<ResourceTexture> resource = App->resources->RequestResource<ResourceTexture>(aux).lock();

	if (ImGui::BeginTable("table1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Image((void*)resource->GetGlTexture(), ImVec2(100, 100));
		ImGui::TableNextColumn();
		ImGui::Text("Width %.2f", resource->GetWidth());
		ImGui::Text("Height %.2f", resource->GetHeight());
		ImGui::EndTable();
	}
	ImGui::Text("");
	if(ImGui::CollapsingHeader("Import Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::shared_ptr<ImportOptionsTexture> importOptions = resource->GetImportOptions();
		
		ImGui::Checkbox("Flip Image", &importOptions->flip);

		ImGui::Text("");
		ImGui::SameLine(ImGui::GetWindowWidth() - 110);
		if (ImGui::Button("Revert"))
		{
			//TODO REVERT
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 50);
		if (ImGui::Button("Apply"))
		{
			resource->Unload();
			resource->SetChanged(true);
			App->resources->ReimportResource(resource->GetUID());
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Load Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::shared_ptr<LoadOptionsTexture> loadOptions = resource->GetLoadOptions();
		ImGui::Checkbox("MipMap", &loadOptions->mipMap);

		const char* minFilters[] = { "NEAREST", "LINEAR", "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR"};
		ImGui::Combo("MinFilter", reinterpret_cast<int*>(&loadOptions->min), minFilters, IM_ARRAYSIZE(minFilters));

		const char* magFilters[] = { "NEAREST", "LINEAR"};
		ImGui::Combo("MagFilter", reinterpret_cast<int*>(&loadOptions->mag), magFilters, IM_ARRAYSIZE(magFilters));

		const char* wrapFilters[] = { "REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER", "MIRROR_REPEAT", "MIRROR_CLAMP_TO_EDGE"};
		ImGui::Combo("WrapFilterS", reinterpret_cast<int*>(&loadOptions->wrapS), wrapFilters, IM_ARRAYSIZE(wrapFilters));
		ImGui::Combo("WrapFilterT", reinterpret_cast<int*>(&loadOptions->wrapT), wrapFilters, IM_ARRAYSIZE(wrapFilters));
	}
}

bool WindowInspectorResource::MousePosIsInWindow()
{
	return (ImGui::GetIO().MousePos.x > ImGui::GetWindowPos().x
		&& ImGui::GetIO().MousePos.x < (ImGui::GetWindowPos().x + ImGui::GetWindowWidth())
		&& ImGui::GetIO().MousePos.y > ImGui::GetWindowPos().y
		&& ImGui::GetIO().MousePos.y < (ImGui::GetWindowPos().y + ImGui::GetWindowHeight()));
}

bool WindowInspectorResource::WindowRightClick()
{
	return (ImGui::GetIO().MouseClicked[1] && MousePosIsInWindow());
}