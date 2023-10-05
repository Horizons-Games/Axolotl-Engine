#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModulePlayer.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/AIMovement.h"

#include "Application.h"
#include "debugdraw.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), aiMovement(nullptr),
	targetTransform(nullptr), ownerTransform(nullptr)
{
}

void SeekBehaviourScript::Start()
{
	target = App->GetModule<ModulePlayer>()->GetPlayer();
	if (target)
	{
		targetTransform = target->GetComponent<ComponentTransform>();
	}
	modulePlayer = App->GetModule<ModulePlayer>();
	ownerTransform = owner->GetComponent<ComponentTransform>();
	aiMovement = owner->GetComponent<AIMovement>();
}

void SeekBehaviourScript::Update(float deltaTime)
{
	if (target != modulePlayer->GetPlayer())
	{
		target = App->GetModule<ModulePlayer>()->GetPlayer();
	}

	if (target)
	{
		targetTransform = target->GetComponent<ComponentTransform>();
	}
}

// When this behaviour is triggered, the enemy will go towards its target
void SeekBehaviourScript::Seeking()
{
	float3 target = targetTransform->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
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
