#include "PlayerForceAttackScript.h"

#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"

#include "EnemyDroneScript.h"

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
		static_cast<ComponentTransform*>(owner->GetParent()->GetComponent(ComponentType::TRANSFORM));
	ComponentRigidBody* rigidBody =
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());

	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && currentCoolDown <= 0)
	{
		currentCoolDown = coolDown;
		const ComponentTransform* transform =
			static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		
	
		for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
			 it++)
		{
			const ComponentTransform* enemyTransform =
				static_cast<ComponentTransform*>((*it)->GetComponent(ComponentType::TRANSFORM));
			ComponentRigidBody* enemyRigidBody =
				static_cast<ComponentRigidBody*>((*it)->GetComponent(ComponentType::RIGIDBODY));

			btRigidBody* enemybtRb = enemyRigidBody->GetRigidBody();
			enemybtRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));


			// Get next position of the gameObject
			float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
			nextPosition.Normalize();
			nextPosition += float3(0, 0.5f, 0);
			nextPosition *= force;
			//nextPosition += transform->GetGlobalPosition();

			btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);

			enemybtRb->setLinearVelocity(newVelocity);
			//enemyRigidBody->SetPositionTarget(nextPosition);
			//enemyRigidBody->SetKpForce(5.0f);
			
			std::vector<ComponentScript*> gameObjectScripts =
				(*it)->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

			for (int i = 0; i < gameObjectScripts.size(); ++i)
			{
				if (gameObjectScripts[i]->GetConstructName() == "EnemyDroneScript")
				{
					static_cast<EnemyDroneScript*>(gameObjectScripts[i]->GetScript())->SetStunnedTime(stunTime);
				}
			}
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
			static_cast<ComponentRigidBody*>((*it)->GetComponent(ComponentType::RIGIDBODY));
		if (rigidBody == other)
		{
			enemiesInTheArea.erase(it);
		}
	}
}
