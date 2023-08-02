#include "StdAfx.h"

#include "WindowComponentTrail.h"
#include "Components/ComponentTrail.h"
#include "ImGui/imgui_color_gradient.h"

#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTrailTexture.h"

#include "Enums/BlendingType.h"


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
		if (ImGui::BeginTable("##trailTable", 2))
		{
			ImGui::TableNextColumn();
			bool onPlay = componentTrail->IsOnPlay();
			ImGui::Text("Play");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::Checkbox("##Onplay", &onPlay))
			{
				componentTrail->SetOnPlay(onPlay);
			}

			float duration = componentTrail->GetDuration();
			ImGui::TableNextColumn();
			ImGui::Text("Duration");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			if (ImGui::DragFloat("##Duration", &duration, 0.1f, 0.1f, 25.f))
			{
				if (duration > 25.f)
				{
					duration = 25.f;
				}
				else if (duration < 0.1f)
				{
					duration = 0.1f;
				}
				componentTrail->SetDuration(duration);
			}
			
			ImGui::TableNextColumn();
			ImGui::Text("Minimum distance");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			
			float minDistance = componentTrail->GetMinDistance();

			if (ImGui::DragFloat("##MinimumDistance", &minDistance, 0.1f, 1.f, 10.f))
			{
				if (minDistance > 10.f)
				{
					minDistance = 10.f;
				}
				else if (minDistance < 0.1f)
				{
					minDistance = 0.1f;
				}
				componentTrail->SetMinDistance(minDistance);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Width");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			float width = componentTrail->GetWidth();
			if (ImGui::DragFloat("##Width", &width, 0.1f, 1.0f, 25.0f))
			{
				if (width > 25.f)
				{
					width = 25.f;
				}
				else if (width < 1.f)
				{
					width = 1.f;
				}
				componentTrail->SetWidth(width);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Blending");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			BlendingMode blending = componentTrail->GetBlendingMode();

			const char* blendingItems[] = { "ALPHA", "ADDITIVE" };
			static const char* currentItem;
			switch (blending)
			{
			case BlendingMode::ALPHA:
				currentItem = blendingItems[0];
				break;
			case BlendingMode::ADDITIVE:
				currentItem = blendingItems[1];
				break;
			}

			if (ImGui::BeginCombo("##blendingCombo", currentItem))
			{
				for (int n = 0; n < IM_ARRAYSIZE(blendingItems); n++)
				{
					bool isSelected = (currentItem == blendingItems[n]);

					if (ImGui::Selectable(blendingItems[n], isSelected))
					{
						currentItem = blendingItems[n];

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				if (currentItem == blendingItems[0])
				{
					blending = BlendingMode::ALPHA;
				}
				else if (currentItem == blendingItems[1])
				{
					blending = BlendingMode::ADDITIVE;
				}

				componentTrail->SetBlendingMode(blending);

				ImGui::EndCombo();
			}
			ImGui::EndTable();
		}

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