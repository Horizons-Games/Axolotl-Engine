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
		std::shared_ptr<ResourceTexture> texture = App->resources->RequestResource<ResourceTexture>(filePath);
		
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
	}
}