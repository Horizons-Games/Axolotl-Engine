#pragma once

#include "../WindowFileBrowser.h"

class ModuleRenderer;

class WindowParticleTexture : public WindowFileBrowser
{
public:
	WindowParticleTexture(ModuleRenderer* moduleRenderer, TextureType textureType);
	~WindowParticleTexture() override;

	void DoThisIfOk() override;

private:
	ModuleRenderer* moduleRenderer;
	TextureType textureType;
};

