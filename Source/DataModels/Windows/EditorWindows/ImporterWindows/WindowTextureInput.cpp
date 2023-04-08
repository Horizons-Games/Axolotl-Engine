#include "WindowTextureInput.h"

#include "Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"

#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput(WindowComponentMeshRenderer* mesh, TextureType textureType) :
	WindowFileBrowser(), windowComponent(mesh), textureType(textureType)
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
		case TextureType::SPECULAR:
			windowComponent->SetSpecular(texture);
			break;
		case TextureType::METALLIC:
			windowComponent->SetMetalic(texture);
			break;
		default:
			break;
		}
	}
}