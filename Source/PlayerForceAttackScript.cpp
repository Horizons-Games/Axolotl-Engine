#include "PlayerForceAttackScript.h"

#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PlayerForceAttackScript);

PlayerForceAttackScript::PlayerForceAttackScript() : Script(), radius(7.0f)
{
	enemiesInTheArea.reserve(10);

	REGISTER_FIELD(radius, float);
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

	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		const ComponentTransform* transform =
			static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		const ComponentRigidBody* rigidBody =
			static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	
		for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
			 it++)
		{
			const ComponentTransform* enemyTransform =
				static_cast<ComponentTransform*>((*it)->GetComponent(ComponentType::TRANSFORM));
			ComponentRigidBody* enemyRigidBody =
				static_cast<ComponentRigidBody*>((*it)->GetComponent(ComponentType::RIGIDBODY));

			// Get next position of the gameObject
			float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
			nextPosition.Normalize();
			nextPosition *= radius;
			nextPosition += transform->GetGlobalPosition();

			enemyRigidBody->SetPositionTarget(nextPosition);
		}
	}
}

void PlayerForceAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	enemiesInTheArea.push_back(other->GetOwner());
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
