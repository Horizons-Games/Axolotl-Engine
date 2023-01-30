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
	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if(resourceAsShared) 
	{
		resourceAsShared->Load();
		//TODO When user select another resource Unload the last one

		ImGui::Text(resourceAsShared->GetFileName().c_str());
		switch (resourceAsShared->GetType())
		{
		case ResourceType::Texture:
			DrawTextureOptions();
			break;
		default:
			break;
		}
	}
}

void WindowInspectorResource::SetResource(const std::weak_ptr<Resource>& resource) {
	std::shared_ptr<Resource> lastResource = this->resource.lock();
	/*if (lastResource) //Unload of last resource
	{
		lastResource->Unload();
	}*/

	this->resource = resource;

	std::shared_ptr<Resource> resourceAsShared = resource.lock();
	if (resourceAsShared)
	{
		switch (resourceAsShared->GetType())
		{
		case ResourceType::Texture:
			InitTextureImportOptions();
			break;
		default:
			break;
		}
	}
}

void WindowInspectorResource::InitTextureImportOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());
	flipVertical = resourceTexture->GetImportOptions()->flipVertical;
	flipHorizontal = resourceTexture->GetImportOptions()->flipHorizontal;
}

void WindowInspectorResource::DrawTextureOptions()
{
	std::shared_ptr<ResourceTexture> resourceTexture = std::dynamic_pointer_cast<ResourceTexture>(resource.lock());

	if (ImGui::BeginTable("table1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Image((void*)resourceTexture->GetGlTexture(), ImVec2(100, 100));
		ImGui::TableNextColumn();
		ImGui::Text("Width %.2f", resourceTexture->GetWidth());
		ImGui::Text("Height %.2f", resourceTexture->GetHeight());
		ImGui::EndTable();
	}
	ImGui::Text("");
	if(ImGui::CollapsingHeader("Import Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Flip Image Vertical", &flipVertical);
		ImGui::Checkbox("Flip Image Horizontal", &flipHorizontal);

		ImGui::Text("");
		ImGui::SameLine(ImGui::GetWindowWidth() - 110);
		if (ImGui::Button("Revert"))
		{
			InitTextureImportOptions();
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 50);
		if (ImGui::Button("Apply"))
		{
			resourceTexture->GetImportOptions()->flipVertical = flipVertical;
			resourceTexture->GetImportOptions()->flipHorizontal = flipHorizontal;
			resourceTexture->Unload();
			resourceTexture->SetChanged(true);
			App->resources->ReimportResource(resourceTexture->GetUID());
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Load Options", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::shared_ptr<LoadOptionsTexture> loadOptions = resourceTexture->GetLoadOptions();
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