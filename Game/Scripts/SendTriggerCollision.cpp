#include "StdAfx.h"
#include "SendTriggerCollision.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SendTriggerCollision);


SendTriggerCollision::SendTriggerCollision() : Script(), rigidbody(nullptr), tagFilter("")
{
	REGISTER_FIELD(tagFilter, std::string);
}

void SendTriggerCollision::Start()
{
	rigidbody = owner->GetComponent<ComponentRigidBody>();
}

void SendTriggerCollision::Update(float deltaTime)
{
	rigidbody->UpdateRigidBody();
}

void SendTriggerCollision::OnCollisionEnter(ComponentRigidBody* other)
{
	if ((tagFilter.empty() || other->GetOwner()->CompareTag(tagFilter)) && collisionEnterCallback)
	{
		collisionEnterCallback(other);
	}
}

void SendTriggerCollision::OnCollisionExit(ComponentRigidBody* other)
{
	if ((tagFilter.empty() || other->GetOwner()->CompareTag(tagFilter)) && collisionExitCallback)
	{
		collisionExitCallback(other);
	}
}

void SendTriggerCollision::SetEnterCollisionCallback(std::function<void(ComponentRigidBody*)>&& callEnterCollision)
{
	collisionEnterCallback = std::move(callEnterCollision);
}

void SendTriggerCollision::SetExitCollisionCallback(std::function<void(ComponentRigidBody*)>&& callExitCollision)
{
	collisionExitCallback = std::move(callExitCollision);
}