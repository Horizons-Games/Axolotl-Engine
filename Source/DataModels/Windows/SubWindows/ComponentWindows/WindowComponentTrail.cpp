#include "StdAfx.h"

#include "WindowComponentTrail.h"
#include "Components/ComponentTrail.h"
#include "ImGui/imgui_color_gradient.h"

#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTrailTexture.h"

#include "Enums/BlendingType.h"

#define DURATION_MIN_VALUE 0.1f
#define DURATION_MAX_VALUE 25.f
#define MIN_DISTANCE_MIN_VALUE 0.5f
#define MIN_DISTANCE_MAX_VALUE 10.f
#define WIDTH_MIN_VALUE 0.05f
#define WIDTH_MAX_VALUE 10.f
#define RATIO_WIDTH_MIN_VALUE 0.f
#define RATIO_WIDTH_MAX_VALUE 1.f
#define CAT_SAMP_MIN_VALUE 0
#define CAT_SAMP_MAX_VALUE 20

WindowComponentTrail::WindowComponentTrail(ComponentTrail* component) : 
	ComponentWindow("TRAIL", component),
	inputTexture(std::make_unique<WindowTrailTexture>(this, TextureType::DIFFUSE)), draggingMark(nullptr),
	selectedMark(nullptr)
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

			if (ImGui::DragFloat("##Duration", &duration, 0.1f, DURATION_MIN_VALUE, DURATION_MAX_VALUE, "%.2f"))
			{
				if (duration > DURATION_MAX_VALUE)
				{
					duration = DURATION_MAX_VALUE;
				}
				else if (duration < DURATION_MIN_VALUE)
				{
					duration = DURATION_MIN_VALUE;
				}
				componentTrail->SetDuration(duration);
			}
			
			ImGui::TableNextColumn();
			ImGui::Text("Minimum distance");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			
			float minDistance = componentTrail->GetMinDistance();

			if (ImGui::DragFloat("##MinimumDistance", &minDistance, 0.05f, MIN_DISTANCE_MIN_VALUE, MIN_DISTANCE_MAX_VALUE, "%.2f"))
			{
				if (minDistance > MIN_DISTANCE_MAX_VALUE)
				{
					minDistance = MIN_DISTANCE_MAX_VALUE;
				}
				else if (minDistance < MIN_DISTANCE_MIN_VALUE)
				{
					minDistance = MIN_DISTANCE_MIN_VALUE;
				}
				componentTrail->SetMinDistance(minDistance);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Width");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			float width = componentTrail->GetWidth();
			if (ImGui::DragFloat("##Width", &width, 0.05f, WIDTH_MIN_VALUE, WIDTH_MAX_VALUE, "%.2f"))
			{
				if (width > WIDTH_MAX_VALUE)
				{
					width = WIDTH_MAX_VALUE;
				}
				else if (width < WIDTH_MIN_VALUE)
				{
					width = WIDTH_MIN_VALUE;
				}
				componentTrail->SetWidth(width);
			}

			float ratioWidht = componentTrail->GetRatioWidth();
			ImGui::TableNextColumn();
			ImGui::Text("Ratio width");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			if (ImGui::DragFloat("##RatioWidth", &ratioWidht, 0.05f, RATIO_WIDTH_MIN_VALUE, RATIO_WIDTH_MAX_VALUE, "%.2f"))
			{
				if (ratioWidht > RATIO_WIDTH_MAX_VALUE)
				{
					ratioWidht = RATIO_WIDTH_MAX_VALUE;
				}
				else if (ratioWidht < RATIO_WIDTH_MIN_VALUE)
				{
					ratioWidht = RATIO_WIDTH_MIN_VALUE;
				}
				componentTrail->SetRatioWidth(ratioWidht);
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

			ImGui::TableNextColumn();
			ImGui::Text("Catmull Samplers");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			
			int numCatmull = componentTrail->GetCatmullPoints();
			if (ImGui::DragInt("##Catmull", &numCatmull, 1.f, CAT_SAMP_MIN_VALUE, CAT_SAMP_MAX_VALUE))
			{
				if (numCatmull > CAT_SAMP_MAX_VALUE)
				{
					numCatmull = CAT_SAMP_MAX_VALUE;
				}
				else if (numCatmull < CAT_SAMP_MIN_VALUE)
				{
					numCatmull = CAT_SAMP_MIN_VALUE;
				}
				componentTrail->SetCatmullPoints(numCatmull);
			}

			ImGui::EndTable();
		}

		ImGui::Separator();

		ImGui::Text("Color Gradient");

		ImGradient* gradient = componentTrail->GetGradient();

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
