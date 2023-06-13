#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), 
		samplePointsObject(nullptr), KpPosition(5.0f), KpRotation(10.0f), 
		rotationSensitivity(1.0f), transform(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
	REGISTER_FIELD(KpPosition, float);
	REGISTER_FIELD(KpRotation, float);
	REGISTER_FIELD(rotationSensitivity, float);
}

void PlayerCameraRotationVerticalScript::Start()
{
	if (samplePointsObject)
	{
		for (auto sample : samplePointsObject->GetChildren())
		{
			CameraSample* sampleScript = sample->GetComponent<CameraSample>();
			samples.push_back(sampleScript);
		}
	}
	transform = owner->GetComponent<ComponentTransform>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	defaultPositionOffset = transform->GetPosition();
	defaultOrientationOffset = transform->GetRotation();
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	Orbit(deltaTime);
}

void PlayerCameraRotationVerticalScript::Orbit(float deltaTime)
{
	Quat currentRotation = transform->GetRotation();
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
	transform->SetRotation(finalRotation);

	float3 cameraOffset = transform->GetPosition();
	cameraOffset = rotationErrorY.Transform(cameraOffset);
	transform->SetPosition(cameraOffset);
	transform->UpdateTransformMatrices();

	defaultPositionOffset = transform->GetPosition();
	defaultOrientationOffset = transform->GetRotation();
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
			targetPosition = closestSample->positionOffset;
			targetOrientation = closestSample->orientationOffset;
		}
		else
		{
			targetPosition = defaultPositionOffset;
			targetOrientation = defaultOrientationOffset;
		}
	}

	else
	{
		targetPosition = defaultPositionOffset;
		targetOrientation = defaultOrientationOffset;
	}

	SetPositionTarget(targetPosition, deltaTime);
	SetRotationTarget(targetOrientation, deltaTime);
	transform->UpdateTransformMatrices();
}


CameraSample* PlayerCameraRotationVerticalScript::FindClosestSample(float3 position)
{
	CameraSample* closestSample = nullptr;
	float minDistance = std::numeric_limits<float>::max();

	for (auto sample : samples)
	{
		float distance = (sample->position - position).Length();
		if (distance < minDistance)
		{
			closestSample = sample;
			minDistance = distance;
		}
	}
	return closestSample;
}



void PlayerCameraRotationVerticalScript::SetPositionTarget(float3 targetPosition, float deltaTime)
{
	float3 currentPosition = transform->GetPosition();

	float3 positionError = targetPosition - currentPosition;
	float3 velocityPosition = positionError * KpPosition;
	float3 nextPos = currentPosition + velocityPosition * deltaTime;
	transform->SetPosition(nextPos);
}

void PlayerCameraRotationVerticalScript::SetRotationTarget(Quat targetRotation, float deltaTime)
{
	Quat currentRotation = transform->GetRotation();

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

	transform->SetRotation(nextRotation);
}
