#include "StdAfx.h"

#include "WindowComponentLine.h"
#include "Components/ComponentLine.h"
#include "ImGui/imgui_color_gradient.h"
#include "ImGui/imgui_bezier.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "DataModels/Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowLineTexture.h"

#include "Math/float4.h"


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
		ImGui::Text("Drop the End Point");
		std::string label;
		if (componentLine->GetEnd())
		{
			label = componentLine->GetEnd()->GetName().c_str();
		}
		else
		{
			label = "";
		}
		std::string finalLabel = label;

		ImGui::Button(finalLabel.c_str(), ImVec2(208.0f, 20.0f));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
			{
				UID draggedGameObjectID = *(UID*)payload->Data;
				GameObject* draggedGameObject =
					App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);

				if (draggedGameObject)
				{
					componentLine->SetEnd(draggedGameObject);
				}
			}

			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Remove End Point"))
		{
			componentLine->SetEnd(nullptr);
		}

		ImGui::Separator();

		float numTiles = componentLine->GetNumTiles();
		ImGui::Text("");
		ImGui::Text("Number of billboards");
		ImGui::SameLine();
		if (ImGui::DragFloat("Billboards", &numTiles, 1.0f, 1.0f, 100.0f))
		{
			
			componentLine->SetNumTiles(numTiles);
			
		}
		float speed = componentLine->GetSpeed();
		ImGui::Text("");
		ImGui::Text("Speed");
		ImGui::SameLine();
		if (ImGui::DragFloat("speed", &speed, 0.1f, -50.0f, 50.0f))
		{

			componentLine->SetSpeed(speed);

		}

		ImGui::Text("");
		ImGui::Text("Size");
		static float4 size = componentLine->GetSizeFadingPoints();
		if(ImGui::Bezier( "", reinterpret_cast<float*>(&size)))
		{
			componentLine->SetSizeFadingPoints(size);
		} 

		ImGui::Text("");
		ImGui::Separator();

		ImGui::Text("");
		ImGui::Text("Fading");
		float2 fadeRange = componentLine->GetSizeFading();
		if (ImGui::DragFloat("Start Fading", &fadeRange.x, 0.01f, 0.0f, 50.0f))
		{
			componentLine->SetSizeFading(fadeRange);
		}
		if (ImGui::DragFloat("End Fading", &fadeRange.y, 0.01f, 0.0f, 50.0f))
		{
			componentLine->SetSizeFading(fadeRange);
		}

		ImGui::Text("");
		ImGui::Separator();

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

		ImGui::Separator();

		if (ImGui::InputFloat2("Tiling", &tiling[0], "%.1f"))
		{
			if (tiling[0] < 0.0f)
			{
				tiling[0] = 0.0f;
			}

			if (tiling[1] < 0.0f)
			{
				tiling[1] = 0.0f;
			}
		};

		if (ImGui::InputFloat2("Offset", &offset[0], "%.3f"))
		{
			if (offset[0] < 0.0f)
			{
				offset[0] = 0.0f;
			}
			else if (offset[0] > 1.0f)
			{
				offset[0] = 1.0f;
			}

			if (offset[1] < 0.0f)
			{
				offset[1] = 0.0f;
			}
			else if (offset[1] > 1.0f)
			{
				offset[1] = 1.0f;
			}
		}

		ImGui::Separator();

		if (ImGui::Button("Apply"))
		{
			componentLine->SetLineTexture(lineTexture);
			componentLine->SetTiling(tiling);
			componentLine->SetOffset(offset);
		}
	}
}

void WindowComponentLine::InitValues()
{
	ComponentLine* componentLine = static_cast<ComponentLine*>(component);
	lineTexture = componentLine->GetLineTexture();
	tiling = componentLine->GetTiling();
	offset = componentLine->GetOffset();
}