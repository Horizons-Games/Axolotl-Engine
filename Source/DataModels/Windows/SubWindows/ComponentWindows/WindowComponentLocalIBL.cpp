#include "StdAfx.h"

#include "WindowComponentLocalIBL.h"

#include "Application.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentLocalIBL.h"

WindowComponentLocalIBL::WindowComponentLocalIBL(ComponentLocalIBL* component) :
	WindowComponentLight("LOCAL IBL", component)
{
}

WindowComponentLocalIBL::~WindowComponentLocalIBL()
{
}

void WindowComponentLocalIBL::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentLocalIBL* local = static_cast<ComponentLocalIBL*>(component);
	if (local)
	{
		if (ImGui::Button("Generate Maps"))
		{
			local->GenerateMaps();
		}

		if (ImGui::BeginTable("##localIBLTable", 2))
		{
			AABB parallax = local->GetParallaxAABB();

			ImGui::TableNextColumn();
			ImGui::Text("Parallax Max");
			ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XParallaxMax", &parallax.maxPoint.x, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YParallaxMax", &parallax.maxPoint.y, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZParallaxMax", &parallax.maxPoint.z, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Parallax Min");
			ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XParallaxMin", &parallax.minPoint.x, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YParallaxMin", &parallax.minPoint.y, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZParallaxMin", &parallax.minPoint.z, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetParallaxAABB(parallax);
			}
			ImGui::PopStyleVar();

			AABB influence = local->GetInfluenceAABB();

			ImGui::TableNextColumn();
			ImGui::Text("Influence Max");
			ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XInfluenceMax", &influence.maxPoint.x, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YInfluenceMax", &influence.maxPoint.y, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZInfluenceMax", &influence.maxPoint.z, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Influence Min");
			ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XInfluenceMin", &influence.minPoint.x, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YInfluenceMin", &influence.minPoint.y, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZInfluenceMin", &influence.minPoint.z, 0.01f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%.2f"))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::PopStyleVar();
			
			ImGui::EndTable();
		}
	}
}
