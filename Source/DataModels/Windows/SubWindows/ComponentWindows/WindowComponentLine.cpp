#include "StdAfx.h"

#include "WindowComponentLine.h"
#include "Components/ComponentLine.h"
#include "ImGui/imgui_color_gradient.h"

#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowLineTexture.h"


WindowComponentLine::WindowComponentLine(ComponentLine* component) :
	ComponentWindow("Line", component),
	inputTexture(std::make_unique<WindowLineTexture>(this, TextureType::DIFFUSE))
{
	InitValues();
}

WindowComponentLine::~WindowComponentLine()
{
}

void WindowComponentLine::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentLine* componentLine = static_cast<ComponentLine*>(component);

	if (componentLine)
	{

		float numTiles = componentLine->GetNumTiles();
		ImGui::Text("");
		ImGui::Text("Number of billboards");
		ImGui::SameLine();
		if (ImGui::DragFloat("Billboards", &numTiles, 1.0f, 1.0f, 50.0f))
		{
			
			componentLine->SetNumTiles(numTiles);
			
		}
		ImGui::Text("");
		ImGui::Text("Color Gradient");
		
		ImGradient* gradient = componentLine->GetGradient();
		static ImGradientMark* draggingMark = nullptr;
		static ImGradientMark* selectedMark = nullptr;

		if (ImGui::GradientEditor(gradient, draggingMark, selectedMark))
		{
			componentLine->SetGradient(gradient);
		}

		ImGui::Separator();

		ImGui::Text("Diffuse Texture");
		if (lineTexture)
		{
			lineTexture->Load();
			ImGui::Image((void*)(intptr_t)lineTexture->GetGlTexture(), ImVec2(100, 100));
			if (ImGui::Button("Remove Texture Diffuse"))
			{
				lineTexture->Unload();
				lineTexture = nullptr;
			}
		}
		else
		{
			inputTexture->DrawWindowContents();
		}

		if (ImGui::Button("Apply"))
		{
			componentLine->SetLineTexture(lineTexture);
		}
	}
}

void WindowComponentLine::InitValues()
{
	ComponentLine* componentLine = static_cast<ComponentLine*>(component);

	lineTexture = componentLine->GetLineTexture();
}