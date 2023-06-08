#include "WindowParticleTexture.h"

#include "Application.h"

#include "FileSystem/ModuleResources.h"

#include "ParticleSystem/ModuleRenderer.h"

#include "Resources/ResourceTexture.h"

WindowParticleTexture::WindowParticleTexture(ModuleRenderer* moduleRenderer, TextureType textureType) :
	WindowFileBrowser(),
	moduleRenderer(moduleRenderer),
	textureType(textureType)
{
	dialogName = "Select Texture";
	startPath = "Assets/Textures";
	title = "Load Diffuse";
	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds *.tif *.tga){.png,.gif,.jpg,.jpeg,.dds,.tif,.tga}";
}

WindowParticleTexture::~WindowParticleTexture()
{
}

void WindowParticleTexture::DoThisIfOk()
{
	if (moduleRenderer)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(filePath);

		moduleRenderer->SetTexture(texture);
	}
}
