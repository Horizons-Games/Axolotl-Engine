#pragma once
#include "../WindowFileBrowser.h"
#include "Enums/TextureType.h"

class WindowComponentTrail;

class WindowTrailTexture : public WindowFileBrowser
{
public:
	WindowTrailTexture(WindowComponentTrail* componentTrail, TextureType textureType);
	~WindowTrailTexture() override;

	void DoThisIfOk() override;

private:
	WindowComponentTrail* componentTrail;
	TextureType textureType;
};
