#include "CameraControllerScript.h"

#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCameraSample.h"

#include "Camera/CameraGameObject.h"

#include "../Scripts/CameraSample.h"

REGISTERCLASS(CameraControllerScript);

CameraControllerScript::CameraControllerScript() : Script(),
		samplePointsObject(nullptr), transform(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
	REGISTER_FIELD_WITH_ACCESSORS(PlayerGameObject, std::vector<GameObject*>);
	REGISTER_FIELD(xOffset, float);
	REGISTER_FIELD(yOffset, float);
	REGISTER_FIELD(zOffset, float);
	REGISTER_FIELD(xFocusOffset, float);
	REGISTER_FIELD(yFocusOffset, float);
	REGISTER_FIELD(inCombat, bool);
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
	camera = GetOwner()->GetComponentInternal<ComponentCamera>();
	Assert(!players.empty(), "Player empty");

	for (GameObject* player : players) 
	{
		Assert(player, "One of the players is null");
	}

	playerTransform = players[currentPlayerIndex]->GetComponent<ComponentTransform>();

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

		if (closestSample->GetKpPositionEnabled())
		{
			camera->SetSampleKpPosition(closestSample->GetKpPosition());
			
		}
		else
		{
			camera->RestoreKpPosition();
		}
		
		if (closestSample->GetKpRotationEnabled())
		{
			camera->SetSampleKpRotation(closestSample->GetKpRotation());
		}
		else
		{
			camera->RestoreKpRotation();
		}
	}
	else
	{
		CalculateOffsetVector();
		CalculateFocusOffsetVector();
		camera->RestoreKpPosition();
		camera->RestoreKpRotation();
	}

	float3 sourceDirection = camera->GetCamera()->GetFrustum()->Front().Normalized();
	float3 targetDirection = (playerTransform->GetGlobalPosition()
		+ defaultFocusOffsetVector
		- camera->GetCamera()->GetPosition()).Normalized();

	Quat orientationOffset = Quat::identity;

	if (!sourceDirection.Cross(targetDirection).Equals(float3::zero, 0.001f))
	{
		Quat rot = Quat::RotateFromTo(sourceDirection, targetDirection);
		orientationOffset = rot * camera->GetCamera()->GetRotation();
	}
	else
	{
		orientationOffset = camera->GetCamera()->GetRotation();
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

void CameraControllerScript::ChangeCurrentPlayer() 
{
	currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
	playerTransform = players[currentPlayerIndex]->GetComponent<ComponentTransform>();
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
	float3 currentFocus = (playerTransform->GetGlobalPosition() - camera->GetCamera()->GetPosition()).Normalized();
	float3 rightVector = currentFocus.Cross(float3::unitY);
	defaultFocusOffsetVector = rightVector * xFocusOffset
		+ float3::unitY * yFocusOffset;
}

void CameraControllerScript::CalculateFocusOffsetVector(float2 offset)
{
	float3 currentFocus = (playerTransform->GetGlobalPosition() - camera->GetCamera()->GetPosition()).Normalized();
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

	if (inCombat && closestCombatSample)
	{
		return closestCombatSample;
	}
	else
	{
		return closestSample;
	}

}

