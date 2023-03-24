#include "WindowTextureInput.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput(ComponentMeshRenderer* material, TextureType textureType) :
	WindowFileBrowser(), meshRendererComponent(material), textureType(textureType)
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
	if (meshRendererComponent)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture = App->resources->RequestResource<ResourceTexture>(filePath);
		
		std::shared_ptr<ResourceMaterial> material = meshRendererComponent->GetMaterial();

		if (material)
		{
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
			//case TextureType::SPECULAR:
			//	material->SetSpecular(texture);
			//	break;
			case TextureType::METALLIC:
				material->SetMetallicMap(texture);
			}
			material->SetChanged(true);
		}
	}
}