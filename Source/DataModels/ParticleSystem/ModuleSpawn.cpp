#include "ModuleSpawn.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleSpawn::ModuleSpawn(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter), spawnRate(0.0f)
{
}

ModuleSpawn::~ModuleSpawn()
{
}

void ModuleSpawn::Spawn(EmitterInstance* instance)
{
}

void ModuleSpawn::Update(EmitterInstance* instance)
{
}

void ModuleSpawn::DrawImGui()
{
	if (ImGui::TreeNodeEx("Spawn Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##spawnTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Spawn rate");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(3.0f, 0.0f)); ImGui::SameLine(0.0f, 0.0f);
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::InputFloat("##spawnRate", &spawnRate, 0.0f, 0.0f, "%.2f"))
			{
				if (spawnRate > MAX_SPAWN_RATE)
				{
					spawnRate = MAX_SPAWN_RATE;
				}
				else if (spawnRate < 0)
				{
					spawnRate = 0;
				}
			}

			ImGui::EndTable();
		}

		ImGui::TreePop();
	}
}
