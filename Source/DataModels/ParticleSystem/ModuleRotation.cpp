#include "ModuleRotation.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleRotation::ModuleRotation(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter),
	random(true), rotationOverTime(float2(-0.5f, 0.5f))
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
				if (particle.rotationOverTime == UNINITIALIZED_ROTATION)
				{
					particle.rotationOverTime = DegToRad(random ?
						instance->CalculateRandomValueInRange(rotationOverTime.x, rotationOverTime.y) :
						rotationOverTime.x);
				}
				particle.rotation += particle.rotationOverTime;
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
		ImGui::SetNextItemWidth(165.0f);
		if (random)
		{
			if (ImGui::SliderFloat2("##sliderRotOverTime", &rotationOverTime[0], -25.0f, 25.0f, "%.2f"))
			{
				if (rotationOverTime.x > rotationOverTime.y)
				{
					rotationOverTime.x = rotationOverTime.y;
				}
				else if (rotationOverTime.x < -25.0f)
				{
					rotationOverTime.x = -25.0f;
				}

				if (rotationOverTime.y < rotationOverTime.x)
				{
					rotationOverTime.y = rotationOverTime.x;
				}
				else if (rotationOverTime.y > 25.0f)
				{
					rotationOverTime.y = 25.0f;
				}
			}
		}
		else
		{
			if (ImGui::InputFloat("##inputRotOverTime", &rotationOverTime.x, 0.01f, 0.1f, "%.2f"))
			{
				if (rotationOverTime.x > 25.0f)
				{
					rotationOverTime.x = 25.0f;
				}
				else if (rotationOverTime.x < -25.0f)
				{
					rotationOverTime.x = -25.0f;
				}
			}
		}
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::Text("Random");
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::Checkbox("##randomRotationOvertime", &random);

		ImGui::TreePop();
	}
	else
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled2", &enabled);
	}
}
