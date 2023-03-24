#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMeshRenderer;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(ComponentMeshRenderer* meshRendererComponent, TextureType textureType);
	~WindowTextureInput() override;

	void DoThisIfOk() override;

private:
	ComponentMeshRenderer* meshRendererComponent;
	TextureType textureType;

	friend class WindowComponentMaterial;
};
