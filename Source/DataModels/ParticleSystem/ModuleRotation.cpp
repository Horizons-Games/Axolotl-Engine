#include "ModuleRotation.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleRotation::ModuleRotation(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter),
	rotationOverTime(0.5f)
{
}

ModuleRotation::~ModuleRotation()
{
}

void ModuleRotation::Spawn(EmitterInstance* instance)
{
}

void ModuleRotation::Update(EmitterInstance* instance)
{
	if (enabled)
	{
		std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

		for (int i = 0; i < particles.size(); ++i)
		{
			EmitterInstance::Particle& particle = particles[i];

			if (particle.lifespan >= 0.0f)
			{
				particle.rotation += DegToRad(rotationOverTime);
			}
		}
	}
}

void ModuleRotation::DrawImGui()
{
	if (ImGui::TreeNodeEx("Rotation Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled2", &enabled);

		ImGui::Text("Rotation over time");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::SliderFloat("##rotation", &rotationOverTime, -25.0f, 25.0f, "%.2f");

		ImGui::TreePop();
	}
	else
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled2", &enabled);
	}
}
