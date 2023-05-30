#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr), 
	ownerRigidBody(nullptr), ownerRigidBodyGO(nullptr), targetTransform(nullptr), ownerTransform(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
	REGISTER_FIELD(ownerRigidBodyGO, GameObject*);
}

void SeekBehaviourScript::Start()
{
	if (target)
	{
		targetTransform = static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));
	}

	ownerRigidBody = static_cast<ComponentRigidBody*>(ownerRigidBodyGO->GetComponent(ComponentType::RIGIDBODY));
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

// When this behaviour is triggered, the enemy will go towards its target
void SeekBehaviourScript::Seeking() const
{
	ownerRigidBody->SetPositionTarget(targetTransform->GetGlobalPosition());
	Quat errorRotation = 
		Quat::RotateFromTo(ownerTransform->GetGlobalForward().Normalized(), 
			(targetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(ownerTransform->GetGlobalPosition(),
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), targetTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

	ownerRigidBody->SetRotationTarget(errorRotation.Normalized());
}

void SeekBehaviourScript::DisableMovement() const
{
	ownerRigidBody->DisablePositionController();
}

void SeekBehaviourScript::DisableRotation() const
{
	ownerRigidBody->DisableRotationController();
}