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
void SeekBehaviourScript::Seeking()
{
	aiMovement->SetTargetPosition(targetTransform->GetGlobalPosition());
	aiMovement->SetMovementStatuses(true, true);
}

void SeekBehaviourScript::DisableMovement()
{
	aiMovement->SetMovementStatuses(false, false);
}

void SeekBehaviourScript::DisableRotation()
{
}

void SeekBehaviourScript::RotateToTarget()
{
}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}
