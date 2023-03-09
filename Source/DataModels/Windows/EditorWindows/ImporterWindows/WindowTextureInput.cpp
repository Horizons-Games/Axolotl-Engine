#include "WindowTextureInput.h"

#include "Components/ComponentMaterial.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput(ComponentMaterial* material, TextureType textureType) :
	WindowFileBrowser(), materialComponent(material), textureType(textureType)
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
	if (materialComponent)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		this->ImportResourceAsync(filePath);
	}
}

void WindowTextureInput::GetResourceAfterImport()
{
	if (materialComponent && this->futureResource.valid())
	{
		std::shared_ptr<ResourceMaterial> materialAsShared = materialComponent->GetMaterial();
		if (materialAsShared)
		{
			std::shared_ptr<ResourceTexture> texture =
				std::dynamic_pointer_cast<ResourceTexture>(this->futureResource.get());
			switch (textureType)
			{
			case TextureType::DIFFUSE:
				materialAsShared->SetDiffuse(texture);
				break;
			case TextureType::NORMAL:
				materialAsShared->SetNormal(texture);
				break;
			case TextureType::OCCLUSION:
				break;
			case TextureType::SPECULAR:
				materialAsShared->SetSpecular(texture);
				break;
			}

			materialAsShared->SetChanged(true);
		}
	}
}
