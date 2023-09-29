#include "StdAfx.h"
#include "WindowComponentPlanarReflection.h"

#include "Components/ComponentPlanarReflection.h"

WindowComponentPlanarReflection::WindowComponentPlanarReflection(ComponentPlanarReflection* component) :
	WindowComponentLight("PLANAR REFLECTION", component)
{
}

WindowComponentPlanarReflection::~WindowComponentPlanarReflection()
{
}

void WindowComponentPlanarReflection::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentPlanarReflection* planar = static_cast<ComponentPlanarReflection*>(component);
	if (planar)
	{
		if (ImGui::Button("Update Reflection"))
		{
			planar->UpdateReflection();
		}

		float3 scale = planar->GetScale();
		if (ImGui::BeginTable("##planarReflectionTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Scaling");
			

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XScale", &scale.x, 0.01f, 0.01f, std::numeric_limits<float>::max(), "%.2f"))
			{
				planar->ScaleInfluenceAABB(scale);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZScale", &scale.z, 0.01f, 0.01f, std::numeric_limits<float>::max(), "%.2f"))
			{
				planar->ScaleInfluenceAABB(scale);
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			float distortionAmount = planar->GetDistortionAmount();
			ImGui::Text("Distortion Amount:");
			
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##DistortionAmount", &distortionAmount, 0.01f, 0.01f, std::numeric_limits<float>::max(), "%.2f"))
			{
				planar->SetDistortionAmount(distortionAmount);
			}
			ImGui::PopStyleVar();

			ImGui::EndTable();
		}
	}
}
