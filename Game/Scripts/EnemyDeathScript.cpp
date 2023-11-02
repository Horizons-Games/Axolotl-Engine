#include "StdAfx.h"
#include "EnemyDeathScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRandom.h"
#include "Modules/ModulePlayer.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/FinalBossScript.h"
#include "../Scripts/MeshEffect.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(EnemyDeathScript);

EnemyDeathScript::EnemyDeathScript() : Script(), despawnTimer(7.5f), startDespawnTimer(false), powerUpParent(nullptr),
particleSystem(nullptr), chanceToGivePowerUp(false), deadBody(nullptr)
{
	REGISTER_FIELD(chanceToGivePowerUp, bool);
	REGISTER_FIELD(powerUpParent, GameObject*);
	REGISTER_FIELD(deadParent, GameObject*);
	REGISTER_FIELD(particleSystem, ComponentParticleSystem*);
}

void EnemyDeathScript::Update(float deltaTime)
{
	if (!startDespawnTimer)
	{
		return;
	}

	despawnTimer -= deltaTime;

	if (despawnTimer <= 5.0f && despawnTimer > 0.f)
	{
		if (deadBody == nullptr)
		{
			deadBody = RequestDeadBody();
			if (deadBody != nullptr)
			{
				owner->GetComponent<MeshEffect>()->EnableDisableMeshes(false);
				deadBody->Enable();
				ComponentTransform* deadTrans = deadBody->GetComponent<ComponentTransform>();
				deadTrans->SetGlobalPosition(owner->GetComponent<ComponentTransform>()->GetGlobalPosition());
				deadTrans->SetGlobalRotation(owner->GetComponent<ComponentTransform>()->GetGlobalRotation());
				deadTrans->RecalculateLocalMatrix();
				deadTrans->UpdateTransformMatrices();
				MeshEffect* meshEffect = deadBody->GetComponent<MeshEffect>();
				meshEffect->AddColor(float4::zero);
				meshEffect->StartEffect(5.f, 0.f);
			}
		}
		if (deadBody)
		{
			deadBody->GetComponent<MeshEffect>()->DisappearBodyEffect();
		}
	}
	else if (despawnTimer <= 0.f)
	{
		DespawnEnemy();
		if (deadBody != nullptr)
		{
			owner->GetComponent<MeshEffect>()->EnableDisableMeshes(true);
			MeshEffect* meshEffect = deadBody->GetComponent<MeshEffect>();
			meshEffect->CleanColors();
			meshEffect->ClearEffect();
			deadBody->Disable();
			deadBody = nullptr;
		}
	}
}

void EnemyDeathScript::ManageEnemyDeath()
{

	if (particleSystem)
	{
		particleSystem->Play();
	}

	if (chanceToGivePowerUp)
	{
		GameObject* newPowerUp = RequestPowerUp();

		if (newPowerUp != nullptr)
		{
			PowerUpLogicScript* newPowerUpLogic = newPowerUp->GetComponent<PowerUpLogicScript>();
			ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();

			newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
		}
	}

	DisableEnemyActions();
	float enemiesLeft = static_cast<float>(App->GetModule<ModulePlayer>()->GetEnemiesToDefeat()) - 1.f;
	App->GetModule<ModulePlayer>()->SetEnemiesToDefeat(enemiesLeft);
}

void EnemyDeathScript::ResetDespawnTimerAndEnableActions()
{
	despawnTimer = 7.5f;
	EnableEnemyActions();
}

GameObject* EnemyDeathScript::RequestPowerUp() const
{
	for (GameObject* selectedPowerUp : powerUpParent->GetChildren())
	{
		// Make that the enemies don't always drop a powerup (20% chance)
		srand(static_cast<unsigned int>(time(0)));
		bool randomActivation = App->GetModule<ModuleRandom>()->RandomChance(5.0f);

		if (!selectedPowerUp->IsEnabled() && randomActivation)
		{
			return selectedPowerUp;
		}
	}

	return nullptr;
}

GameObject* EnemyDeathScript::RequestDeadBody()
{
	for (auto selectedDead : deadParent->GetChildren())
	{
		if (!selectedDead->IsEnabled())
		{
			return selectedDead;
		}
	}

	return nullptr;
}

void EnemyDeathScript::DisableEnemyActions()
{
	MeshEffect* meshEffectScript = owner->GetComponent<HealthSystem>()->GetMeshEffect();
	meshEffectScript->ClearEffect();

	// Once the enemy is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		if (script->GetConstructName() != "EnemyDeathScript")
		{
			script->Disable();
		}
	}

	ComponentRigidBody* enemyRigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyRigidBody->Disable();
	enemyRigidBody->SetIsKinematic(true);
	enemyRigidBody->SetUpMobility();

	startDespawnTimer = true;
}

void EnemyDeathScript::EnableEnemyActions()
{
	// If the enemy is revived, enable its scripts again
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		if (script->GetConstructName() != "EnemyDeathScript")
		{
			script->Enable();
		}
	}

	ComponentRigidBody* enemyRigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyRigidBody->Enable();
	enemyRigidBody->SetIsKinematic(false);
	enemyRigidBody->SetUpMobility();

	startDespawnTimer = false;
}

void EnemyDeathScript::DespawnEnemy() const
{
	//App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	owner->Disable();
}
