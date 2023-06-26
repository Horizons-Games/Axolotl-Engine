#include "ModuleSize.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleSize::ModuleSize(ParticleEmitter* emitter) :
	ParticleModule(ModuleType::SIZE, emitter),
	random(true), sizeOverTime(float2(0.001, 0.01))
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
				if (particle.sizeOverTime == -1.0f)
				{
					particle.sizeOverTime = random ? 
						instance->CalculateRandomValueInRange(sizeOverTime.x, sizeOverTime.y) : sizeOverTime.x;
				}

				particle.size += particle.sizeOverTime;
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
		ImGui::SetNextItemWidth(165.0f);
		if (random)
		{
			if (ImGui::SliderFloat2("##sliderSizeOverTime", &sizeOverTime[0], 0.0f, MAX_SIZE, "%.3f"))
			{
				if (sizeOverTime.x > sizeOverTime.y)
				{
					sizeOverTime.x = sizeOverTime.y;
				}
				else if (sizeOverTime.x < 0.0f)
				{
					sizeOverTime.x = 0.0f;
				}

				if (sizeOverTime.y < sizeOverTime.x)
				{
					sizeOverTime.y = sizeOverTime.x;
				}
				else if (sizeOverTime.y > MAX_SIZE)
				{
					sizeOverTime.y = MAX_SIZE;
				}
			}
		}
		else
		{
			if (ImGui::InputFloat("##inputSizeOverTime", &sizeOverTime.x, 0.001f, 0.01f, "%.3f"))
			{
				if (sizeOverTime.x > MAX_SIZE)
				{
					sizeOverTime.x = MAX_SIZE;
				}
				else if (sizeOverTime.x < 0.0f)
				{
					sizeOverTime.x = 0.0f;
				}
			}
		}
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::Text("Random");
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::Checkbox("##randomSizeOverTime", &random);

		ImGui::TreePop();
	}
	else
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled3", &enabled);
	}
}
