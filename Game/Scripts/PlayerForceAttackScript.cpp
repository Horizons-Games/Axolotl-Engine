#include "PlayerForceAttackScript.h"

#include "ModuleInput.h"

#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"

#include "EnemyDroneScript.h"
#include "HealthSystem.h"

REGISTERCLASS(PlayerForceAttackScript);

PlayerForceAttackScript::PlayerForceAttackScript() : Script(), force(10.0f), stunTime(3.0f), coolDown(6.0f), 
	currentCoolDown(0.0f)
{
	enemiesInTheArea.reserve(10);

	REGISTER_FIELD(force, float);
	REGISTER_FIELD(stunTime, float);
	REGISTER_FIELD(coolDown, float);
}

PlayerForceAttackScript::~PlayerForceAttackScript()
{
}

void PlayerForceAttackScript::Start()
{
}

void PlayerForceAttackScript::Update(float deltaTime)
{
	const ModuleInput* input = App->GetModule<ModuleInput>();
	
	const ComponentTransform* parentTransform =
		owner->GetParent()->GetComponent<ComponentTransform>();
	ComponentRigidBody* rigidBody =
		owner->GetComponent<ComponentRigidBody>();

	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());

	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && currentCoolDown <= 0)
	{
		currentCoolDown = coolDown;
		const ComponentTransform* transform =
			owner->GetComponent<ComponentTransform>();
		
	
		for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
			 it++)
		{
			const ComponentTransform* enemyTransform =
				(*it)->GetComponent<ComponentTransform>();
			ComponentRigidBody* enemyRigidBody =
				(*it)->GetComponent<ComponentRigidBody>();

			btRigidBody* enemybtRb = enemyRigidBody->GetRigidBody();
			enemyRigidBody->DisablePositionController();
			enemyRigidBody->DisableRotationController();
			enemybtRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
			enemybtRb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));


			// Get next position of the gameObject
			float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
			nextPosition.Normalize();
			nextPosition += float3(0, 0.5f, 0);
			nextPosition *= force;

			btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
			enemybtRb->setLinearVelocity(newVelocity);
			
			EnemyDroneScript* enemyDroneScript =
				(*it)->GetComponent<EnemyDroneScript>();

			enemyDroneScript->SetStunnedTime(stunTime);		

			HealthSystem* enemyHealthScript =
				(*it)->GetComponent<HealthSystem>();

			enemyHealthScript->TakeDamage(10.0f);
		}
	}
	else
	{
		if (currentCoolDown > 0) 
		{
			currentCoolDown -= deltaTime;
			currentCoolDown = std::max(0.0f, currentCoolDown);
		}
	}

	ENGINE_LOG("Objetos: %i", enemiesInTheArea.size());
}

void PlayerForceAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy")
	{
		enemiesInTheArea.push_back(other->GetOwner());
	}
}

void PlayerForceAttackScript::OnCollisionExit(ComponentRigidBody* other)
{
	for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end(); it++)
	{
		const ComponentRigidBody* rigidBody =
			(*it)->GetComponent<ComponentRigidBody>();
		if (rigidBody == other)
		{
			enemiesInTheArea.erase(it);
		}
	}
}
