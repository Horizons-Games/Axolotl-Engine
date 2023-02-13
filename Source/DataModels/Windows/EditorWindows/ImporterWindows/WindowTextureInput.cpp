#include "WindowTextureInput.h"
#include "imgui.h"

#include "Components/ComponentMaterial.h"
#include "Resources/ResourceMaterial.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowTextureInput::WindowTextureInput(ComponentMaterial* material,
	TextureType textureType) :
	WindowFileBrowser(),
	materialComponent(material),
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
		UID uidTexture = App->resources->ImportResource(filePath);

		std::shared_ptr<ResourceMaterial> materialAsShared = materialComponent->GetMaterial().lock();

		if (materialAsShared)
		{
			switch (textureType)
			{
			case TextureType::DIFFUSE:
				materialAsShared->SetDiffuseUID(uidTexture);
				break;
			case TextureType::NORMAL:
				materialAsShared->SetNormalUID(uidTexture);
				break;
			case TextureType::OCCLUSION:
				break;
			case TextureType::SPECULAR:
				materialAsShared->SetSpecularUID(uidTexture);
				break;
			}

			materialAsShared->SetChanged(true);
		}
	}
}