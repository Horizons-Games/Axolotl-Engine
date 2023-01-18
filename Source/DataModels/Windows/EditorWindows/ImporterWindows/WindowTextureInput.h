#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h";
#include <memory>

class ResourceMaterial;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(const std::weak_ptr<ResourceMaterial>& material, TextureType textureType);
	~WindowTextureInput() = default;
	void DoThisIfOk() override;

private:
	std::weak_ptr<ResourceMaterial> material;
	TextureType textureType;
};

inline WindowTextureInput::WindowTextureInput(const std::weak_ptr<ResourceMaterial>& material, TextureType textureType) :WindowFileBrowser()
{
	this->material = material;
	this->textureType = textureType;

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

	filters = "Image files (*.png *.gif *.jpg *.jpeg *.dds){.png,.gif,.jpg,.jpeg,.dds}";
	startPath = "Assets/Textures";
}
