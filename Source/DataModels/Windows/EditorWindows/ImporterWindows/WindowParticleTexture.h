#pragma once

#include "../WindowFileBrowser.h"

#include "Enums/TextureType.h"

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

