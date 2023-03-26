#include "WindowComponentImage.h"

#include "Components/ComponentImage.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowSpriteInput.h"
#include "Resources/ResourceTexture.h"

#include "Application.h"


WindowComponentImage::WindowComponentImage(ComponentImage* component) :
	ComponentWindow("IMAGE", component), inputImage(std::make_unique<WindowSpriteInput>(component))
{
}

WindowComponentImage::~WindowComponentImage()
{
}

void WindowComponentImage::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentImage* asImage = static_cast<ComponentImage*>(component);
	std::shared_ptr<ResourceTexture> image = asImage->GetImage();
	if (image)
	{
		image->Load();
		ImGui::Image((void*)(intptr_t)image->GetGlTexture(), ImVec2(100, 100));
		if (ImGui::Button("Remove Texture Diffuse"))
		{
			image->Unload();
			asImage->SetImage(nullptr);
		}
	}
	else
	{
		inputImage->DrawWindowContents();
	}
}
