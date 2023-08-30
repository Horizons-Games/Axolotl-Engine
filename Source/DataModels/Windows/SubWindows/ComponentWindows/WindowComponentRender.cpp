#include "StdAfx.h"

#include "WindowComponentRender.h"

#include "Components/ComponentRender.h"

#define INTENSITY_MIN_VALUE 0.f
#define INTENSITY_MAX_VALUE 25.f

WindowComponentRender::WindowComponentRender(ComponentRender* component) : ComponentWindow("MODULE RENDER", component)
{
}

WindowComponentRender::~WindowComponentRender()
{
}

void WindowComponentRender::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentRender* componentRender = static_cast<ComponentRender*>(component);

	if (componentRender)
	{
		if (ImGui::BeginTable("##renderTable", 2))
		{
			ImGui::TableNextColumn();
			float intensity = componentRender->GetBloomIntensity();
			ImGui::Text("Bloom Intensity");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::DragFloat("##BloomIntensity", &intensity, 0.05f, INTENSITY_MIN_VALUE, INTENSITY_MAX_VALUE, "%.2f"))
			{
				componentRender->SetBloomIntensity(intensity);
			}

			ImGui::EndTable();
		}
	}
}
