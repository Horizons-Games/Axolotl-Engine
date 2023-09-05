#include "StdAfx.h"
#include "WindowTrailTexture.h"
#include "Application.h"

#include "FileSystem/ModuleResources.h"

#include "Windows/SubWindows/ComponentWindows/WindowComponentTrail.h"

#include "Resources/ResourceTexture.h"

WindowTrailTexture::WindowTrailTexture(WindowComponentTrail* componentTrail, TextureType textureType) :
	WindowFileBrowser(),
	componentTrail(componentTrail),
	textureType(textureType)
{
	dialogName = "Select Texture";
	startPath = "Assets/Textures";
	title = "Load Diffuse";
	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds *.tif *.tga){.png,.gif,.jpg,.jpeg,.dds,.tif,.tga}";
}

WindowTrailTexture::~WindowTrailTexture()
{
}

void WindowTrailTexture::DoThisIfOk()
{
	if (componentTrail)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(filePath);

		componentTrail->SetTexture(texture);
	}
}