#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h"
#include <memory>

class ComponentMaterial;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(const std::weak_ptr<ComponentMaterial>& material, TextureType textureType);
	~WindowTextureInput();
	void DoThisIfOk() override;

private:
	std::weak_ptr<ComponentMaterial> materialComponent;
	TextureType textureType;

	friend class WindowComponentMaterial;
};

inline WindowTextureInput::WindowTextureInput(const std::weak_ptr<ComponentMaterial>& material,
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

inline WindowTextureInput::~WindowTextureInput()
{
}