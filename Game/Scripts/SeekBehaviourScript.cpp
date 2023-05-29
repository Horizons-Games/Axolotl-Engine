#include "SeekBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "debugdraw.h"

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
	LOG_INFO("Now seeking...");
	ComponentTransform* ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	// When this behaviour is triggered, the enemy will go towards its target

	ownerRigidBody->SetPositionTarget(targetTransform->GetGlobalPosition());
	Quat targetRotation = 
		Quat::RotateFromTo(ownerTransform->GetGlobalForward(), 
			(targetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

	dd::arrow(ownerTransform->GetGlobalPosition(), ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward()*5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), targetTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
	ownerRigidBody->SetRotationTarget(targetRotation);
	ownerRigidBody->SetKpTorque(15.0f);

}

GameObject* SeekBehaviourScript::GetTarget() const
{
	return target;
}

void SeekBehaviourScript::SetTarget(GameObject* target)
{
	this->target = target;
}