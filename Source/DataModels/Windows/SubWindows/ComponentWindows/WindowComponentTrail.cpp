#include "StdAfx.h"

#include "WindowComponentTrail.h"
#include "Components/ComponentTrail.h"
#include "ImGui/imgui_color_gradient.h"

#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTrailTexture.h"


WindowComponentTrail::WindowComponentTrail(ComponentTrail* component) : 
			ComponentWindow("TRAIL", component),
			inputTexture(std::make_unique<WindowTrailTexture>(this, TextureType::DIFFUSE))
{
	Init();
}

WindowComponentTrail::~WindowComponentTrail()
{
}

void WindowComponentTrail::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentTrail* componentTrail = static_cast<ComponentTrail*>(component);

	if (componentTrail)
	{
		float duration = componentTrail->GetDuration();
		ImGui::Text("");
		ImGui::Text("Duration");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Duration", &duration, 0.1f, 1.0f, 100.0f))
		{
			componentTrail->SetDuration(duration);
		}
		float minDistance = componentTrail->GetMinDistance();
		ImGui::Text("");
		ImGui::Text("Minimum distance");
		ImGui::SameLine();
		if (ImGui::DragFloat("##MinimumDistance", &minDistance, 0.1f, 1.0f, 100.0f))
		{
			componentTrail->SetMinDistance(minDistance);
		}

		ImGui::Text("");
		ImGui::Text("Width");
		ImGui::SameLine();
		float width = componentTrail->GetWidth();
		if (ImGui::DragFloat("##Width", &width, 0.1f, 1.0f, 100.0f))
		{
			componentTrail->SetWidth(width);
		}
		ImGui::Text("");
		ImGui::Separator();

		ImGui::Text("Color Gradient");

		ImGradient* gradient = componentTrail->GetGradient();
		static ImGradientMark* draggingMark = nullptr;
		static ImGradientMark* selectedMark = nullptr;

		if (ImGui::GradientEditor(gradient, draggingMark, selectedMark))
		{
			componentTrail->SetGradient(gradient);
		}

		ImGui::Separator();

		ImGui::Text("Diffuse Texture");
		if (texture)
		{
			texture->Load();
			ImGui::Image((void*) (intptr_t) texture->GetGlTexture(), ImVec2(100, 100));
			if (ImGui::Button("Remove Texture Diffuse"))
			{
				texture->Unload();
				texture = nullptr;
			}
		}
		else
		{
			inputTexture->DrawWindowContents();
		}

		if (ImGui::Button("Apply"))
		{
			componentTrail->SetTexture(texture);
		}
	}

}

void WindowComponentTrail::Init()
{
	ComponentTrail* componentTrail = static_cast<ComponentTrail*>(component);
	texture = componentTrail->GetTexture();
}
