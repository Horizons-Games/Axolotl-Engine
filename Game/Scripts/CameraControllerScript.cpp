#include "CameraControllerScript.h"

#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCameraSample.h"

#include "../Scripts/CameraSample.h"

REGISTERCLASS(CameraControllerScript);

CameraControllerScript::CameraControllerScript() : Script(),
		samplePointsObject(nullptr), transform(nullptr), player(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
	REGISTER_FIELD(player, GameObject*);
	REGISTER_FIELD(xOffset, float);
	REGISTER_FIELD(yOffset, float);
	REGISTER_FIELD(zOffset, float);
	REGISTER_FIELD(xFocusOffset, float);
	REGISTER_FIELD(yFocusOffset, float);
}

void CameraControllerScript::Start()
{
	if (samplePointsObject)
	{
		for (GameObject* sample : samplePointsObject->GetChildren())
		{
			samples.push_back(sample->GetComponent<ComponentCameraSample>());
		}
	}
	transform = owner->GetComponent<ComponentTransform>();
	playerTransform = player->GetComponent<ComponentTransform>();

	finalTargetPosition = transform->GetGlobalPosition();
	finalTargetOrientation = transform->GetGlobalRotation();
	CalculateOffsetVector();
	CalculateFocusOffsetVector();
}

void CameraControllerScript::PreUpdate(float deltaTime)
{

	ComponentCameraSample* closestSample = FindClosestSample(playerTransform->GetGlobalPosition());

	if (closestSample)
	{
		CalculateOffsetVector(closestSample->GetOffset());
		CalculateFocusOffsetVector(closestSample->GetFocusOffset());
	}
	else
	{
		CalculateOffsetVector();
		CalculateFocusOffsetVector();
	}

	float3 sourceDirection = transform->GetGlobalForward().Normalized();
	float3 targetDirection = (playerTransform->GetGlobalPosition()
		+ defaultFocusOffsetVector
		- transform->GetGlobalPosition()).Normalized();

	Quat orientationOffset = Quat::identity;

	if (!sourceDirection.Cross(targetDirection).Equals(float3::zero, 0.01))
	{
		Quat rot = Quat::RotateFromTo(sourceDirection, targetDirection);
		orientationOffset = rot * transform->GetGlobalRotation();
	}
	else
	{
		orientationOffset = transform->GetGlobalRotation();
	}

	finalTargetPosition = playerTransform->GetGlobalPosition() + defaultOffsetVector;
	finalTargetOrientation = orientationOffset;

	transform->SetGlobalPosition(finalTargetPosition);
	transform->SetGlobalRotation(finalTargetOrientation);
	transform->RecalculateLocalMatrix();
}

void CameraControllerScript::CalculateOffsetVector()
{
	defaultOffsetVector = float3::unitX * xOffset 
		+ float3::unitY * yOffset 
		+ float3::unitZ * zOffset;
}

void CameraControllerScript::CalculateOffsetVector(float3 offset)
{
	defaultOffsetVector = offset;
}

void CameraControllerScript::CalculateFocusOffsetVector()
{
	defaultFocusOffsetVector = float3::unitX * xFocusOffset
		+ float3::unitY * yFocusOffset
		+ float3::unitZ * 0.0f;
}

void CameraControllerScript::CalculateFocusOffsetVector(float2 offset)
{

	defaultFocusOffsetVector = float3(offset, 0.0f);
}


ComponentCameraSample* CameraControllerScript::FindClosestSample(float3 position)
{
	ComponentCameraSample* closestSample = nullptr;
	float minDistance = std::numeric_limits<float>::max();

	for (auto sample : samples)
	{
		float distance = (sample->GetPosition() - position).Length();
		if (distance < minDistance && distance <= sample->GetRadius())
		{
			closestSample = sample;
			minDistance = distance;
		}
	}
	return closestSample;
}
