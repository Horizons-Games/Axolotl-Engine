#include "ModuleSize.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleSize::ModuleSize(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter),
	sizeOverTime(0.005f)
{
}

ModuleSize::~ModuleSize()
{
}

void ModuleSize::Spawn(EmitterInstance* instance)
{
}

void ModuleSize::Update(EmitterInstance* instance)
{
	if (enabled)
	{
		std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

		for (int i = 0; i < particles.size(); ++i)
		{
			EmitterInstance::Particle& particle = particles[i];

			if (particle.lifespan >= 0.0f)
			{
				particle.size += sizeOverTime;
			}
		}
	}
}

void ModuleSize::DrawImGui()
{
	if (ImGui::TreeNodeEx("Size Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled3", &enabled);

		ImGui::Text("Size over time");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::SliderFloat("##sizeOverTime", &sizeOverTime, 0.0f, 0.1, "%.3f");

		ImGui::TreePop();
	}
	else
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled3", &enabled);
	}
}
