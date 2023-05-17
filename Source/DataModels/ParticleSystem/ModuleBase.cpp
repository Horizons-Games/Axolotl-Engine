#include "ModuleBase.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleBase::ModuleBase() : ParticleModule(ModuleType::BASE)
{
	originLocation = DEFAULT_ORIGIN;
	alignment = Alignment::WORLD;
}

ModuleBase::~ModuleBase()
{
}

void ModuleBase::Spawn(EmitterInstance* emitter)
{
}

void ModuleBase::Update(EmitterInstance* emitter)
{
}

void ModuleBase::DrawImGui()
{
	if (ImGui::TreeNode("Base Module"))
	{
		if (ImGui::BeginTable("##baseTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Origin"); ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XTrans", &originLocation.x, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YTrans", &originLocation.y, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZTrans", &originLocation.z, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar();
		}
	}
}
