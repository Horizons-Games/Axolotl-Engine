#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr)
{
	REGISTER_FIELD_WITH_ACCESSORS(Target, GameObject*);
}

void SeekBehaviourScript::Start()
{
	if (target)
	{
		targetTransform = target->GetComponent<ComponentTransform>();
	}
	ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
}

void SeekBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Now seeking...");
	ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();
	// When this behaviour is triggered, the enemy will go towards its target

	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());
	ownerRigidBody->SetRotationTarget(targetTransform->GetRotation());

}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}

void SeekBehaviourScript::SetTarget(GameObject* target)
{
	this->target = target;
}