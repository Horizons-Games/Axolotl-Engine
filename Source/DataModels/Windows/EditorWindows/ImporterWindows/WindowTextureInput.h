#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentMaterial;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(ComponentMaterial* material, TextureType textureType);
	~WindowTextureInput() override;

	void DoThisIfOk() override;

protected:
	void GetResourceAfterImport() override;

private:
	ComponentMaterial* materialComponent;
	TextureType textureType;

	friend class WindowComponentMaterial;
};
