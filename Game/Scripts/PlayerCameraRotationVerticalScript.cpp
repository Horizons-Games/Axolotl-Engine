#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), 
		KpPosition(5.0f), KpRotation(10.0f), rotationSensitivity(0.0001f), transform(nullptr)
{
	REGISTER_FIELD(KpPosition, float);
	REGISTER_FIELD(KpRotation, float);
	REGISTER_FIELD(rotationSensitivity, float);
}

void PlayerCameraRotationVerticalScript::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	defaultPositionOffset = float3(0.0f, 0.0f, -5.0f); // por ejemplo
	defaultOrientationOffset = Quat::identity;
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	Orbit(deltaTime);
}

void PlayerCameraRotationVerticalScript::Update(float deltaTime)
{
	float3 targetPosition;
	Quat targetOrientation;

	CameraSample* closestSample = FindClosestSample(transform->GetGlobalPosition());

	if (closestSample)
	{
		if ((closestSample->position - transform->GetGlobalPosition()).Length() <= closestSample->influenceRadius)
		{
			targetPosition = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalPosition() + closestSample->positionOffset;
			targetOrientation = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalRotation() * closestSample->orientationOffset;
		}
		else
		{
			targetPosition = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalPosition() + defaultPositionOffset;
			targetOrientation = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalRotation() * defaultOrientationOffset;
		}
	}

	else
	{
		targetPosition = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalPosition() + defaultPositionOffset;
		targetOrientation = owner->GetParent()->GetComponent<ComponentTransform>()->GetGlobalRotation() * defaultOrientationOffset;
	}

	SetPositionTarget(targetPosition, deltaTime);
	SetRotationTarget(targetOrientation, deltaTime);
}


void PlayerCameraRotationVerticalScript::Orbit(float deltaTime)
{
	Quat currentRotation = transform->GetGlobalRotation();
	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * deltaTime * rotationSensitivity;

	/*float verticalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().y * deltaTime * rotationSensitivity;

	float minPitch = DegToRad(-80.0f);
	float maxPitch = DegToRad(80.0f);
	float pitch = currentRotation.ToEulerXYZ().x;

	if (pitch + verticalMotion < minPitch)
		verticalMotion = minPitch - pitch;
	else if (pitch + verticalMotion > maxPitch)
		verticalMotion = maxPitch - pitch;

	Quat rotationErrorX = Quat::RotateX(verticalMotion);*/


	Quat rotationErrorY = Quat::RotateY(-horizontalMotion);

	//Quat rotationErrorQuat = rotationErrorX.Mul(rotationErrorY);

	Quat finalRotation = rotationErrorY * currentRotation;
	transform->SetGlobalRotation(finalRotation);

	ComponentTransform* parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	float3 vectorOffset = parentTransform->GetGlobalPosition() - transform->GetGlobalPosition();
	vectorOffset = rotationErrorY.Transform(vectorOffset);
	transform->SetGlobalPosition(parentTransform->GetGlobalPosition() - vectorOffset);

	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
}

CameraSample* PlayerCameraRotationVerticalScript::FindClosestSample(float3 position)
{
	CameraSample* closestSample = nullptr;
	float minDistance = std::numeric_limits<float>::max();

	for (auto& sample : samples)
	{
		float distance = (sample.position - position).Length();
		if (distance < minDistance)
		{
			closestSample = &sample;
			minDistance = distance;
		}
	}
	return closestSample;
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
