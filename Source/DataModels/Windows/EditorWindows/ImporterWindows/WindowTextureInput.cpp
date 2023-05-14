#include "WindowTextureInput.h"
#include "Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput
	(WindowComponentMeshRenderer* material, TextureType textureType) :
	WindowFileBrowser(), windowComponent(material), textureType(textureType)
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
	if (windowComponent)
	{
		this->isLoading = false;
		std::string filePath = 
			std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture = 
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(filePath);

		switch (textureType)
		{
		case TextureType::DIFFUSE:

			windowComponent->SetDiffuse(texture);

			break;

		case TextureType::NORMAL:

			windowComponent->SetNormal(texture);

			break;
		case TextureType::OCCLUSION:

			break;

		case TextureType::METALLIC:

			windowComponent->SetMetallic(texture);

			break;

		case TextureType::SPECULAR:

			windowComponent->SetSpecular(texture);

			break;
		}
	}
}
