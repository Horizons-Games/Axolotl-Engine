#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/AIMovement.h"

#include "debugdraw.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr), aiMovement(nullptr),
	targetTransform(nullptr), ownerTransform(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
}

void SeekBehaviourScript::Start()
{
	if (target)
	{
		targetTransform = target->GetComponent<ComponentTransform>();
	}
	ownerTransform = owner->GetComponent<ComponentTransform>();
	aiMovement = owner->GetComponent<AIMovement>();
}

// When this behaviour is triggered, the enemy will go towards its target
void SeekBehaviourScript::Seeking() const
{
	aiMovement->SetTargetPosition(targetTransform->GetGlobalPosition(), true);
	/*ownerRigidBody->SetPositionTarget(targetTransform->GetGlobalPosition());
	RotateToTarget();*/
}

void SeekBehaviourScript::DisableMovement() const
{
	aiMovement->SetMovementStatus(false);
}

void SeekBehaviourScript::DisableRotation() const
{
	//ownerRigidBody->DisableRotationController();
}

void SeekBehaviourScript::RotateToTarget() const
{
	/*Quat errorRotation =
		Quat::RotateFromTo(ownerTransform->GetGlobalForward().Normalized(),
			(targetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());*/

#ifdef DEBUG
	dd::arrow(ownerTransform->GetGlobalPosition(),
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), targetTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

	//ownerRigidBody->SetRotationTarget(errorRotation);
}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}
