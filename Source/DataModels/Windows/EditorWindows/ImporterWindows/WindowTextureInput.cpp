#include "WindowTextureInput.h"

#include "Windows/SubWindows/ComponentWindows/WindowComponentMaterial.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput(WindowComponentMaterial* material, TextureType textureType) :
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
	/*case TextureType::SPECULAR:
		title = "Load Specular";
		break;*/
	case TextureType::METALLIC:
		title = "Load Metallic";
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
				windowComponent->SetDiffuse(texture);
				break;
			case TextureType::NORMAL:
				windowComponent->SetNormal(texture);
				break;
			case TextureType::OCCLUSION:
				break;
			case TextureType::METALLIC:
				windowComponent->SetMetalic(texture);
				break;
			default:
				break;
			}

			materialAsShared->SetChanged(true);
		}
	}
}
