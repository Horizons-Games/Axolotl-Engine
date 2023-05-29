#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr), 
	ownerRigidBody(nullptr), targetTransform(nullptr)
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
}

void SeekBehaviourScript::Seeking() const
{
	// When this behaviour is triggered, the enemy will go towards its target
	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());
	ownerRigidBody->SetRotationTarget(targetTransform->GetRotation());
}