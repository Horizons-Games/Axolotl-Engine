#include "SeekingBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(SeekingBehaviourScript);

SeekingBehaviourScript::SeekingBehaviourScript() : Script(), target(nullptr)
{
	REGISTER_FIELD(Target, GameObject*);
}

void SeekingBehaviourScript::Start()
{
	//targetTransform = static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));
	//ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
}

void SeekingBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Now seeking...");

	// Ideally, these values should be set in the Start() as this script's variables
	ComponentTransform* targetTransform =
		static_cast<ComponentTransform*>(target->GetComponent(ComponentType::TRANSFORM));

	ComponentRigidBody* ownerRigidBody =
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	// ----------------

	// When this behaviour is triggered, the enemy will go towards its target
	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());
	ownerRigidBody->SetRotationTarget(Quat(targetTransform->GetRotation()));
}

GameObject* SeekingBehaviourScript::GetTarget() const
{
	return target;
}

void SeekingBehaviourScript::SetTarget(GameObject* target)
{
	this->target = target;
}