#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "../Scripts/CameraSample.h"

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
		for (GameObject* sample : samplePointsObject->GetChildren())
		{
			samples.push_back(sample->GetComponent<CameraSample>());
		}
	}
	transform = owner->GetComponent<ComponentTransform>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	defaultPosition = transform->GetPosition();
	defaultOffset = defaultPosition.Length();
	defaultOrientation = transform->GetRotation();
}


void PlayerCameraRotationVerticalScript::Orbit(float deltaTime)
{
	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * deltaTime * rotationSensitivity;

	Quat currentRotation = transform->GetRotation();
	Quat rotationErrorY = Quat::RotateY(-horizontalMotion);
	defaultOrientation = rotationErrorY * currentRotation;

	float3 cameraVector = transform->GetPosition().Normalized();
	defaultPosition = rotationErrorY.Transform(cameraVector)*defaultOffset;
}



void PlayerCameraRotationVerticalScript::Update(float deltaTime)
{
	Orbit(deltaTime);

	float3 finalPositionTarget = defaultPosition;
	Quat finalRotationTarget = defaultOrientation;

	CameraSample* closestSample = FindClosestSample(transform->GetGlobalPosition());
	if (closestSample && (closestSample->position - transform->GetGlobalPosition()).Length() <= closestSample->influenceRadius)
	{
		float newOffset = (closestSample->position - parentTransform->GetGlobalPosition()).Length();
		finalPositionTarget = defaultPosition.Normalized() * newOffset;		
	}

	SetPositionTarget(finalPositionTarget, deltaTime);
	SetRotationTarget(finalRotationTarget, deltaTime);

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
	
	float3 positionError = targetPosition - defaultPosition;
	float3 velocityPosition = positionError * KpPosition;
	float3 translationVector = velocityPosition * deltaTime;
	if (translationVector.Length() > positionError.Length())
	{
		transform->SetPosition(targetPosition);
	}
	else
	{
		float3 nextPos = defaultPosition + translationVector;
		transform->SetPosition(nextPos);
	}
	
}

void PlayerCameraRotationVerticalScript::SetRotationTarget(Quat targetRotation, float deltaTime)
{
	Quat rotationError = targetRotation * defaultOrientation.Inverted();
	rotationError.Normalize();
	
	float3 axis;
	float angle;
	rotationError.ToAxisAngle(axis, angle);
	axis.Normalize();

	float3 velocityRotation = axis * angle * KpRotation;
	Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
	Quat wq_0 = angularVelocityQuat * defaultOrientation;

	float deltaValue = 0.5f * deltaTime;
	Quat deltaRotation =
		Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

	if (deltaRotation.Length() > rotationError.Length())
	{
		transform->SetRotation(targetRotation);
	}

	else
	{
		Quat nextRotation(defaultOrientation.x + deltaRotation.x,
			defaultOrientation.y + deltaRotation.y,
			defaultOrientation.z + deltaRotation.z,
			defaultOrientation.w + deltaRotation.w);
		nextRotation.Normalize();

		transform->SetRotation(nextRotation);
	}

	
}
