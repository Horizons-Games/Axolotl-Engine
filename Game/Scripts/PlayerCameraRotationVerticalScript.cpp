#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), 
		KpPosition(5.0f), KpRotation(10.0f), rotationSensitivity(50.0f)
{
	REGISTER_FIELD(KpPosition, float);
	REGISTER_FIELD(KpRotation, float);
	REGISTER_FIELD(rotationSensitivity, float);
}

void PlayerCameraRotationVerticalScript::Start()
{
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	Rotation(deltaTime);
}

void PlayerCameraRotationVerticalScript::Rotation(float deltaTime)
{
	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * deltaTime;
	float verticalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().y * deltaTime;
}

void PlayerCameraRotationVerticalScript::SetPositionTarget(float3 targetPosition, float deltaTime)
{
	float3 currentPosition = transform->GetGlobalPosition();

	float3 positionError = targetPosition - currentPosition;
	float3 velocityPosition = positionError * KpPosition;
	float3 nextPos = currentPosition + velocityPosition * deltaTime;
	transform->SetGlobalPosition(nextPos);
}

void PlayerCameraRotationVerticalScript::SetRotationTarget(Quat targetRotation, float deltaTime)
{
	Quat currentRotation = transform->GetGlobalRotation();

	Quat rotationError = targetRotation * currentRotation.Normalized().Inverted();
	rotationError.Normalize();
	
	float3 axis;
	float angle;
	rotationError.ToAxisAngle(axis, angle);
	axis.Normalize();

	float3 velocityRotation = axis * angle * KpRotation;
	Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
	Quat wq_0 = angularVelocityQuat * currentRotation;

	float deltaValue = 0.5f * deltaTime;
	Quat deltaRotation =
		Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

	Quat nextRotation(currentRotation.x + deltaRotation.x,
		currentRotation.y + deltaRotation.y,
		currentRotation.z + deltaRotation.z,
		currentRotation.w + deltaRotation.w);
	nextRotation.Normalize();

	transform->SetGlobalRotation(nextRotation);
}
