#include "WindowTextureInput.h"
#include "imgui.h"

#include "Components/ComponentMaterial.h"
#include "Resources/ResourceMaterial.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"


void WindowTextureInput::DoThisIfOk()
{
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	UID uidTexture = this->ImportResourceWithLoadingWindow(filePath);

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