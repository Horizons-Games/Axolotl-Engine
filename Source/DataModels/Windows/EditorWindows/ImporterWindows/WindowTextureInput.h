#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "Globals.h";
#include <memory>

class ComponentMaterial;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(ComponentMaterial* material, TextureType textureType);
	~WindowTextureInput();
	void DoThisIfOk() override;

private:
	ComponentMaterial* materialComponent;
	TextureType textureType;
};

inline WindowTextureInput::WindowTextureInput(ComponentMaterial* material, TextureType textureType) :WindowFileBrowser()
{
	this->materialComponent = material;
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

inline WindowTextureInput::~WindowTextureInput()
{
}