#pragma once
#include "ComponentWindow.h"

class ComponentImage;
class WindowSpriteInput;

class WindowComponentImage : public ComponentWindow
{
public:
	WindowComponentImage(ComponentImage* component);
	~WindowComponentImage() override;

protected:
	void DrawWindowContents() override;

private:
	std::unique_ptr<WindowSpriteInput> inputImage;
};
