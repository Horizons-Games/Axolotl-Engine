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
			ImGui::TableNextColumn();
			AABB parallax = local->GetParallaxAABB();
			ImGui::Text("Parallax min");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::DragFloat3("##ParallaxMin", (float*)&parallax.minPoint, 0.001f))
			{
				local->SetParallaxAABB(parallax);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Parallax max");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::DragFloat3("##ParallaxMax", (float*)&parallax.maxPoint, 0.001f))
			{
				local->SetParallaxAABB(parallax);
			}

			AABB influence = local->GetInfluenceAABB();
			ImGui::TableNextColumn();
			ImGui::Text("Influence min");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::DragFloat3("##InfluenceMin", (float*)&influence.minPoint, 0.001f))
			{
				local->SetInfluenceAABB(influence);
			}

			ImGui::TableNextColumn();
			ImGui::Text("Influence max");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			if (ImGui::DragFloat3("##InfluenceMax", (float*)&influence.maxPoint, 0.001f))
			{
				local->SetInfluenceAABB(influence);
			}
			ImGui::EndTable();
		}
	}
}
