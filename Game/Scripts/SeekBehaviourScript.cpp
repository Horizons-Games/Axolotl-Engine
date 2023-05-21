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
		targetTransform = static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));
	}
	ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
}

void SeekBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("Now seeking...");
	ComponentTransform* ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
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