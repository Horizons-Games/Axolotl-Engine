#include "StdAfx.h"
#include "PlayerForceAttackScript.h"

#include "ModuleInput.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyClass.h"

REGISTERCLASS(PlayerForceAttackScript);

PlayerForceAttackScript::PlayerForceAttackScript() : Script(), force(10.0f), stunTime(3.0f), coolDown(6.0f), 
	currentCoolDown(0.0f), forceDamage(10.0f)
{
	enemiesInTheArea.reserve(10);

	REGISTER_FIELD(force, float);
	REGISTER_FIELD(stunTime, float);
	REGISTER_FIELD(coolDown, float);
	REGISTER_FIELD(forceDamage, float);

}

PlayerForceAttackScript::~PlayerForceAttackScript()
{
}

void PlayerForceAttackScript::Start()
{
	input = App->GetModule<ModuleInput>();
	rigidBody =	owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
}

void PlayerForceAttackScript::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());

	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && currentCoolDown <= 0)
	{
		currentCoolDown = coolDown;
		PushEnemies();
	}
	else
	{
		if (currentCoolDown > 0) 
		{
			currentCoolDown -= deltaTime;
			currentCoolDown = std::max(0.0f, currentCoolDown);
		}
	}
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
	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const GameObject* gameObject)
		{
			return gameObject == other->GetOwner();
		}
		),
		std::end(enemiesInTheArea));
}

void PlayerForceAttackScript::PushEnemies()
{
	const ComponentTransform* transform =
		owner->GetComponent<ComponentTransform>();

	for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
		it++)
	{
		const ComponentTransform* enemyTransform =
			(*it)->GetComponent<ComponentTransform>();
		ComponentRigidBody* enemyRigidBody =
			(*it)->GetComponent<ComponentRigidBody>();

		btRigidBody* enemybtRigidbody = enemyRigidBody->GetRigidBody();
		enemyRigidBody->DisablePositionController();
		enemyRigidBody->DisableRotationController();
		enemybtRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		enemybtRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));


		// Get next position of the gameObject
		float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
		nextPosition.Normalize();
		nextPosition += float3(0, 0.5f, 0);
		nextPosition *= force;

		btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
		enemybtRigidbody->setLinearVelocity(newVelocity);

		EnemyClass* enemyScript = (*it)->GetComponent<EnemyClass>();
		enemyScript->SetStunnedTime(stunTime);

		HealthSystem* enemyHealthScript =
			(*it)->GetComponent<HealthSystem>();

		enemyHealthScript->TakeDamage(forceDamage);
	}
}
