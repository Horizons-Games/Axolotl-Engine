#include "ModuleSpawn.h"

#include "EmitterInstance.h"
#include "ParticleEmitter.h"

#include "Application.h"

#include "ImGui/imgui.h"

ModuleSpawn::ModuleSpawn(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter),
	spawnRate(DEFAULT_SPAWN_RATE)
{
}

ModuleSpawn::~ModuleSpawn()
{
}

void ModuleSpawn::Spawn(EmitterInstance* instance)
{
	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();
	unsigned lastParticleUsed = instance->GetLastParticleUsed();
	bool found = false;

	for (unsigned i = lastParticleUsed; !found && i < particles.size(); ++i)
	{
		found = particles[i].lifespan == 0.0f;
		if (found)
		{
			lastParticleUsed = i;
		}
	}

	for (unsigned i = 0; !found && i < lastParticleUsed; ++i)
	{
		found = particles[i].lifespan == 0.0f;
		if (found)
		{
			lastParticleUsed = i;
		}
	}

	if (found)
	{
		EmitterInstance::Particle& particle = particles[lastParticleUsed];

		float2 size = emitter->GetSizeRange();
		float2 rotation = emitter->GetRotationRange();
		float2 life = emitter->GetLifespanRange();
		float2 gravity = emitter->GetGravityRange();

		particle.initColor = emitter->GetColor();
		particle.initSize = emitter->IsRandomSize() ? instance->CalculateRandomValueInRange(size.x, size.y) : size.x;
		particle.initRotation = emitter->IsRandomRot() ?
			instance->CalculateRandomValueInRange(rotation.x, rotation.y) : rotation.x;
		particle.initLife = emitter->IsRandomLife() ? instance->CalculateRandomValueInRange(life.x, life.y) : life.x;
		particle.lifespan = particle.initLife;
		particle.gravity = emitter->IsRandomGravity() ? 
			instance->CalculateRandomValueInRange(gravity.x, gravity.y) : gravity.x;

		instance->SetAliveParticles(instance->GetAliveParticles() + 1);
	}
}

void ModuleSpawn::Update(EmitterInstance* instance)
{
	float dt = App->GetDeltaTime();
	
	if (spawnRate > 0)
	{
		float lastEmission = instance->GetLastEmission() + dt;
		float emissionPeriod = 1.0f / spawnRate;
		int maxParticles = emitter->GetMaxParticles();
		
		while (instance->GetAliveParticles() < maxParticles && (lastEmission - emissionPeriod > 0.0f))
		{
			Spawn(instance);
			lastEmission -= emissionPeriod;
		}

		instance->SetLastEmission(lastEmission);
	}
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
			if (ImGui::InputFloat("##spawnRate", &spawnRate, 0.1f, 1.0f, "%.2f"))
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
