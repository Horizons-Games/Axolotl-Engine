#include "StdAfx.h"

#include "Application.h"
#include "EmitterInstance.h"
#include "ParticleEmitter.h"
#include "ModuleSpawn.h"

#include "Camera/Camera.h"

#include "Modules/ModuleCamera.h"

#include "ImGui/imgui.h"

ModuleSpawn::ModuleSpawn(ParticleEmitter* emitter) :
	ParticleModule(ModuleType::SPAWN, emitter), spawnRate(DEFAULT_SPAWN_RATE)
{
}

ModuleSpawn::ModuleSpawn(ParticleEmitter* emitter, ModuleSpawn* spawn) :
	ParticleModule(ModuleType::SPAWN, emitter)
{
	spawnRate = spawn->GetSpawnRate();
}

ModuleSpawn::~ModuleSpawn()
{
}

void ModuleSpawn::Spawn(EmitterInstance* instance)
{
	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();
	unsigned lastParticleUsed = instance->GetLastParticleUsed();
	bool found = false;

	// Higher probability for the new particle to be spawned, to be after the last particle used
	for (unsigned i = lastParticleUsed; !found && i < particles.size(); ++i)
	{
		found = particles[i].dead;
		if (found)
		{
			lastParticleUsed = i;
		}
	}

	for (unsigned i = 0; !found && i < lastParticleUsed; ++i)
	{
		found = particles[i].dead;
		if (found)
		{
			lastParticleUsed = i;
		}
	}

	if (found)
	{
		EmitterInstance::Particle& particle = particles[lastParticleUsed];

		float2 size = instance->GetSizeRange();
		float2 rotation = instance->GetRotationRange();
		float2 life = instance->GetLifespanRange();
		float2 gravity = instance->GetGravityRange();

		particle.initColor = particle.color = instance->GetColor();
		particle.initSize = particle.size = instance->IsRandomSize() ? 
			instance->CalculateRandomValueInRange(size.x, size.y) : size.x;
		particle.sizeOverTime = -1.0f;
		particle.initRotation = particle.rotation = DegToRad(instance->IsRandomRot() ?
			instance->CalculateRandomValueInRange(rotation.x, rotation.y) : rotation.x);
		particle.rotationOverTime = UNINITIALIZED_ROTATION;
		particle.initLife = particle.lifespan = instance->IsRandomLife() ? 
			instance->CalculateRandomValueInRange(life.x, life.y) : life.x;
		particle.gravity = instance->IsRandomGravity() ? 
			instance->CalculateRandomValueInRange(gravity.x, gravity.y) : gravity.x;
		particle.frame = -1.0f;
		particle.dead = false;

		instance->SetAliveParticles(instance->GetAliveParticles() + 1);

		// Calculate the order of drawing of the new particle relative to the camera
		float3 cameraPos = App->GetModule<ModuleCamera>()->GetSelectedCamera()->GetPosition();
		particle.distanceToCamera = particle.tranform.TranslatePart().DistanceSq(cameraPos);

		std::vector<unsigned int> sortedPos = instance->GetSortedPositions();

		std::vector<unsigned int>::iterator it =
			std::lower_bound(sortedPos.begin(), sortedPos.end(), lastParticleUsed,
				[particles](const unsigned int& a, const unsigned int& b)
				{
					return particles[a].distanceToCamera > particles[b].distanceToCamera;
				});

		sortedPos.insert(it, lastParticleUsed);
		instance->SetSortedPositions(sortedPos);
	}
}

void ModuleSpawn::Update(EmitterInstance* instance)
{
	float dt = App->GetDeltaTime();

	float elapsed = instance->GetElapsedTime();
	elapsed += dt;
	instance->SetElapsedTime(elapsed);
	
	if ((elapsed <= instance->GetDuration() || instance->IsLooping()) && spawnRate > 0)
	{
		float lastEmission = instance->GetLastEmission() + dt;
		float emissionPeriod = 1.0f / spawnRate;
		unsigned int maxParticles = instance->GetMaxParticles();
		
		while (instance->GetAliveParticles() < maxParticles && (lastEmission - emissionPeriod > 0.0f))
		{
			Spawn(instance);
			lastEmission -= emissionPeriod;
		}

		instance->SetLastEmission(lastEmission);
	}
}

void ModuleSpawn::CopyConfig(ParticleModule* module)
{
	ModuleSpawn* spawn = static_cast<ModuleSpawn*>(module);

	enabled = spawn->IsEnabled();
	spawnRate = spawn->GetSpawnRate();
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
			ImGui::SetNextItemWidth(160.0f);
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
