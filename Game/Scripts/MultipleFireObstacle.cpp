#include "StdAfx.h"
#include "MultipleFireObstacle.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/SendTriggerCollision.h"

REGISTERCLASS(MultipleFireObstacle);

MultipleFireObstacle::MultipleFireObstacle() : Script(), deactivatedTime(1.0f), activatedTime(1.0f) , timer(0.0f)
{
	REGISTER_FIELD(fireGroup1, std::vector<GameObject*>);
	REGISTER_FIELD(fireGroup2, std::vector<GameObject*>);
	REGISTER_FIELD(activatedTime, float);
	REGISTER_FIELD(deactivatedTime, float);
}

void MultipleFireObstacle::Start()
{
	for (GameObject* fire : fireGroup1)
	{
		fire->GetComponent<SendTriggerCollision>()->
			SetEnterCollisionCallback(std::bind(&MultipleFireObstacle::CollisionDetected, this, std::placeholders::_1));
	}
	for (GameObject* fire : fireGroup2)
	{
		fire->GetComponent<SendTriggerCollision>()->
			SetEnterCollisionCallback(std::bind(&MultipleFireObstacle::CollisionDetected, this, std::placeholders::_1));

		fire->GetComponent<ComponentRigidBody>()->Disable();
		fire->GetComponent<ComponentParticleSystem>()->Disable();
	}

	fireState = FireState::GROUP1_ACTIVE;
	timer = activatedTime;
}

void MultipleFireObstacle::Update(float deltaTime)
{
	timer -= deltaTime;

	if (timer <= 0.0f)
	{
		switch (fireState)
		{
		case FireState::GROUP1_ACTIVE:
			for (GameObject* fire : fireGroup1)
			{
				fire->GetComponent<ComponentRigidBody>()->Disable();
				fire->GetComponent<ComponentParticleSystem>()->Disable();
			}
			timer = deactivatedTime;
			fireState = FireState::GROUP2_WAITING_TO_ACTIVATE;
			break;

		case FireState::GROUP2_ACTIVE:
			for (GameObject* fire : fireGroup2)
			{
				fire->GetComponent<ComponentRigidBody>()->Disable();
				fire->GetComponent<ComponentParticleSystem>()->Disable();
			}
			timer = deactivatedTime;
			fireState = FireState::GROUP1_WAITING_TO_ACTIVATE;
			break;

		case FireState::GROUP1_WAITING_TO_ACTIVATE:
			for (GameObject* fire : fireGroup1)
			{
				fire->GetComponent<ComponentRigidBody>()->Enable();
				fire->GetComponent<ComponentParticleSystem>()->Enable();
			}
			timer = activatedTime;
			fireState = FireState::GROUP1_ACTIVE;
			break;

		case FireState::GROUP2_WAITING_TO_ACTIVATE:
			for (GameObject* fire : fireGroup2)
			{
				fire->GetComponent<ComponentRigidBody>()->Enable();
				fire->GetComponent<ComponentParticleSystem>()->Enable();
			}
			timer = activatedTime;
			fireState = FireState::GROUP2_ACTIVE;
			break;
		}
	}
}

void MultipleFireObstacle::CollisionDetected(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("PlayerSpaceshipMesh"))
	{
		other->GetOwner()->GetParent()->GetComponent<HealthSystem>()->TakeDamage(1.0f);
	}
}