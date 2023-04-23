#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SeekBehaviourScript);

SeekBehaviourScript::SeekBehaviourScript() : Script(), target(nullptr)
{
	REGISTER_FIELD(Target, GameObject*);
}

void SeekBehaviourScript::Start()
{
	targetTransform = static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));
	ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
}

void SeekBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Now seeking...");

	// When this behaviour is triggered, the enemy will go towards its target
	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());
	ownerRigidBody->SetRotationTarget(Quat(targetTransform->GetRotation()));
}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}

void SeekBehaviourScript::SetTarget(GameObject* target)
{
	this->target = target;
}