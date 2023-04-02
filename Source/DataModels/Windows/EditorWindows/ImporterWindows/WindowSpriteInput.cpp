#include "WindowSpriteInput.h"

#include "Application.h"
#include "Components/UI/ComponentImage.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceTexture.h"

WindowSpriteInput::WindowSpriteInput(ComponentImage* image) : WindowFileBrowser(), component(image)
{
	dialogName = "Select Texture";

	title = "Load Image";

	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds *.tif *.tga){.png,.gif,.jpg,.jpeg,.dds,.tif,.tga}";
	startPath = "Assets/Textures";
}

WindowSpriteInput::~WindowSpriteInput()
{
}

void WindowSpriteInput::DoThisIfOk()
{
	if (component)
	{
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceTexture> texture = App->resources->RequestResource<ResourceTexture>(filePath);
		component->SetImage(texture);
	}
}
