#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowComponentMaterial;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(WindowComponentMaterial* material, TextureType textureType);
	~WindowTextureInput() override;

	void DoThisIfOk() override;

private:
	WindowComponentMaterial* windowComponent;
	TextureType textureType;

	friend class WindowComponentMaterial;
};
