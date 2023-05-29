#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"

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
	ownerRigidBody->SetPositionTarget(targetTransform->GetGlobalPosition());
	Quat targetRotation = 
		Quat::RotateFromTo(ownerTransform->GetGlobalForward(), 
			(targetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

	dd::arrow(ownerTransform->GetGlobalPosition(), 
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward()*5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), targetTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
	ownerRigidBody->SetRotationTarget(targetRotation);
	ownerRigidBody->SetKpTorque(15.0f);
}

void SeekBehaviourScript::StopSeeking() const
{
	ownerRigidBody->SetPositionTarget(ownerTransform->GetPosition());
}