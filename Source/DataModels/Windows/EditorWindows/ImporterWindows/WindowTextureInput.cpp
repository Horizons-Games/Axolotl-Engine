#include "StdAfx.h"

#include "WindowTextureInput.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"

WindowTextureInput::WindowTextureInput(WindowComponentMeshRenderer* material, TextureType textureType) :
	WindowFileBrowser(),
	windowComponent(material),
	textureType(textureType)
{
	dialogName = "Select Texture";

	switch (textureType)
	{
	case TextureType::DIFFUSE:
		title = "Load Diffuse";
		break;
	case TextureType::NORMAL:
		title = "Load Normal";
		break;
	case TextureType::OCCLUSION:
		title = "Load Occlusion";
		break;
	case TextureType::METALLIC:
		title = "Load Metallic";
		break;
	case TextureType::SPECULAR:
		title = "Load Specular";
		break;
	case TextureType::EMISSION:
		title = "Load Emission";
		break;
	default:
		break;
	}
	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds *.tif *.tga){.png,.gif,.jpg,.jpeg,.dds,.tif,.tga}";
	startPath = "Assets/Textures";
}
WindowTextureInput::~WindowTextureInput()
{
}

void WindowTextureInput::DoThisIfOk()
{
	std::shared_ptr<ResourceMaterial> material = windowComponent->GetMaterial();
	if (material)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(filePath);
		
		texture->GetImportOptions().compression = TextureCompression::NONE;
		
		switch (textureType)
		{
			case TextureType::DIFFUSE:
				texture->GetImportOptions().compression = TextureCompression::BC1;

				break;

			case TextureType::NORMAL:

				texture->GetImportOptions().compression = TextureCompression::BC5;

				break;
			case TextureType::OCCLUSION:


				break;

			case TextureType::METALLIC:

				texture->GetImportOptions().compression = TextureCompression::BC5;
				break;

			case TextureType::SPECULAR:

				texture->GetImportOptions().compression = TextureCompression::BC3;

				break;
			default:
				texture->GetImportOptions().compression = TextureCompression::NONE;

				break;
		}
		App->GetModule<ModuleResources>()->ReimportResource(texture->GetUID());
		windowComponent->MaterialChanged();

		switch (textureType)
		{
			case TextureType::DIFFUSE:

				material->SetDiffuse(texture);

				break;

			case TextureType::NORMAL:

				material->SetNormal(texture);

				break;
			case TextureType::OCCLUSION:

				break;

			case TextureType::METALLIC:

				material->SetMetallic(texture);

				break;

			case TextureType::SPECULAR:

				material->SetSpecular(texture);

				break;
			case TextureType::EMISSION:

				material->SetEmission(texture);

				break;
		}
	}
}
