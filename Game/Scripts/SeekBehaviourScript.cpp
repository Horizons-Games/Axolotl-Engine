#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr), 
	ownerRigidBody(nullptr), targetTransform(nullptr), ownerTransform(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
}

void SeekBehaviourScript::Start()
{
	if (target)
	{
		targetTransform = static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));
	}

	ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

// When this behaviour is triggered, the enemy will go towards its target
void SeekBehaviourScript::Seeking() const
{
	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());
	
	Quat targetRotation = Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
		(targetTransform->GetPosition() - ownerTransform->GetPosition()).Normalized());
	ownerRigidBody->SetRotationTarget(targetRotation);
}

void SeekBehaviourScript::StopSeeking() const
{
	ownerRigidBody->SetPositionTarget(ownerTransform->GetPosition());
}