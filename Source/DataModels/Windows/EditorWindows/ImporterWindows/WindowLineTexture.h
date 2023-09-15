#pragma once

#include "Enums/TextureType.h"
#include "../WindowFileBrowser.h"

class WindowComponentLine;

class WindowLineTexture : public WindowFileBrowser
{
public:
	WindowLineTexture(WindowComponentLine* componentline, TextureType textureType);
	~WindowLineTexture() override;

	void DoThisIfOk() override;

private:
	WindowComponentLine* componentline;
	TextureType textureType;
};

