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
		if (closestSample->GetFixedOffsetEnabled())
		{
			CalculateOffsetVector(closestSample->GetFixedOffset() - playerTransform->GetGlobalPosition());
		}
		else
		{
			CalculateOffsetVector(closestSample->GetOffset());
		}
		
		if (closestSample->GetFocusOffsetEnabled())
		{
			CalculateFocusOffsetVector(closestSample->GetFocusOffset());
		}
		else
		{
			CalculateFocusOffsetVector();
		}
		
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

	if (!sourceDirection.Cross(targetDirection).Equals(float3::zero, 0.01f))
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

	for (Component* components : owner->GetComponents())
	{
		components->OnTransformChanged();
	}
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
	float3 currentFocus = (playerTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
	float3 rightVector = currentFocus.Cross(float3::unitY);
	defaultFocusOffsetVector = rightVector * xFocusOffset
		+ float3::unitY * yFocusOffset;
}

void CameraControllerScript::CalculateFocusOffsetVector(float2 offset)
{
	float3 currentFocus = (playerTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
	float3 rightVector = currentFocus.Cross(float3::unitY);
	defaultFocusOffsetVector = rightVector * offset.x
		+ float3::unitY * offset.y;
}


ComponentCameraSample* CameraControllerScript::FindClosestSample(float3 position)
{
	ComponentCameraSample* closestSample = nullptr;
	ComponentCameraSample* closestCombatSample = nullptr;

	float minDistance = std::numeric_limits<float>::max();
	float minCombatDistance = std::numeric_limits<float>::max();

	for (ComponentCameraSample* sample : samples)
	{
		float distance = (sample->GetPosition() - position).Length();

		if (distance <= sample->GetRadius())
		{
			if (sample->GetCombatCameraEnabled() && distance < minCombatDistance)
			{
				closestCombatSample = sample;
				minCombatDistance = distance;
			}
			else if (distance < minDistance)
			{
				closestSample = sample;
				minDistance = distance;
			}
		}
	}

	if (closestCombatSample)
		return closestCombatSample;
	else
		return closestSample;
}

