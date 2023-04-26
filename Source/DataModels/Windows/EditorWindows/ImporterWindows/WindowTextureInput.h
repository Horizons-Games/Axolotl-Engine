#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowComponentMeshRenderer;

class WindowTextureInput :
    public WindowFileBrowser
{
public:
	WindowTextureInput(WindowComponentMeshRenderer* meshRendererComponent, TextureType textureType);
	~WindowTextureInput() override;

	void DoThisIfOk() override;

protected:
	void GetResourceAfterImport() override;

private:
	WindowComponentMeshRenderer* windowComponent;
	TextureType textureType;

	friend class WindowComponentMeshRenderer;
};
