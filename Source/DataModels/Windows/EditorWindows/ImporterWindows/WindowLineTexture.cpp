#include "StdAfx.h"
#include "WindowLineTexture.h"

#include "Application.h"

#include "FileSystem/ModuleResources.h"

#include "Windows/SubWindows/ComponentWindows/WindowComponentLine.h"

#include "Resources/ResourceTexture.h"

WindowLineTexture::WindowLineTexture(WindowComponentLine* componentline, TextureType textureType):
	WindowFileBrowser(),
	componentline(componentline),
	textureType(textureType)
{
	dialogName = "Select Texture";
	startPath = "Assets/Textures";
	title = "Load Diffuse";
	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds *.tif *.tga){.png,.gif,.jpg,.jpeg,.dds,.tif,.tga}";

}

WindowLineTexture::~WindowLineTexture()
{
}

void WindowLineTexture::DoThisIfOk()
{
	if (componentline)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(filePath);

		componentline->SetTexture(texture);
	}
}
