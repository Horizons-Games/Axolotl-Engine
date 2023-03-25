#include "WindowComponentImage.h"

#include "Components/ComponentImage.h"

#include "Application.h"


WindowComponentImage::WindowComponentImage(ComponentImage* component) :
	ComponentWindow("IMAGE", component)
{
}

WindowComponentImage::~WindowComponentImage()
{
}

void WindowComponentImage::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
}
