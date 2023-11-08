#include "StdAfx.h"
#include "SpaceshipDeathManager.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/SpaceshipMovement.h"
#include "../Scripts/HealthSystem.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(SpaceshipDeathManager);

SpaceshipDeathManager::SpaceshipDeathManager() : Script(), componentAudioSource(nullptr), spaceshipMesh(nullptr),
deathVFX(nullptr)
{
	REGISTER_FIELD(spaceshipMesh, GameObject*);
	REGISTER_FIELD(deathVFX, ComponentParticleSystem*);
}

void SpaceshipDeathManager::Start()
{
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();
}

void SpaceshipDeathManager::ManageSpaceshipDeath()
{
	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::EXPLOSION);
	deathVFX->Play();

	SpaceshipMovement* moveScript = owner->GetComponent<SpaceshipMovement>();
	moveScript->Stop();
	moveScript->Disable();

	HealthSystem* healthScript = owner->GetComponent<HealthSystem>();
	healthScript->Disable();

	spaceshipMesh->Disable();
	//DisablePlayerActions();
}