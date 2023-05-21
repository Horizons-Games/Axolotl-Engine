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

	// When this behaviour is triggered, the enemy will go towards its target
	ownerRigidBody->SetPositionTarget(targetTransform->GetPosition());

	Quat xCorrectness(0.7071f, 0.0f, 0.0f, 0.7071f);
	ownerRigidBody->SetRotationTarget(Quat(targetTransform->GetRotation()) * xCorrectness);
}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}

void SeekBehaviourScript::SetTarget(GameObject* target)
{
	this->target = target;
}