#pragma once

#include "ParticleModule.h"

#define MAX_SPAWN_RATE 50

class ModuleSpawn : public ParticleModule
{
public:
	ModuleSpawn();
	~ModuleSpawn() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;

	void DrawImGui() override;

	void SetSpawnRate(float spawnRate);

	float GetSpawnRate() const;

private:
	float spawnRate;
};

inline void ModuleSpawn::SetSpawnRate(float spawnRate)
{
	this->spawnRate = spawnRate;
}

inline float ModuleSpawn::GetSpawnRate() const
{
	return spawnRate;
}
