#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;
class ComponentParticleSystem;
class GameObject;

class SpaceshipDeathManager : public Script
{
public:
	SpaceshipDeathManager();
	~SpaceshipDeathManager() override = default;

	void Start() override;

	void ManageSpaceshipDeath();

private:
	ComponentAudioSource* componentAudioSource;
	ComponentParticleSystem* deathVFX;
	GameObject* spaceshipMesh;
};


