#include "CameraControllerScript.h"

#include "ModuleInput.h"
#include "ModulePlayer.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCameraSample.h"
#include "Components/ComponentAudioSource.h"
#include "Auxiliar/Audio/AudioData.h"

#include "Camera/CameraGameObject.h"
#include "Application.h"

REGISTERCLASS(CameraControllerScript);

CameraControllerScript::CameraControllerScript() : Script(),
		samplePointsObject(nullptr), transform(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
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
			ComponentCameraSample* componentSample = sample->GetComponent<ComponentCameraSample>();
			if (componentSample->IsEnabled())
			{
				samples.push_back(componentSample);
			}
			
		}
	}
	transform = owner->GetComponent<ComponentTransform>();
	camera = GetOwner()->GetComponentInternal<ComponentCamera>();

	playerTransform = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentTransform>(); 

	finalTargetPosition = transform->GetGlobalPosition();
	finalTargetOrientation = transform->GetGlobalRotation();
	CalculateOffsetVector();
	CalculateFocusOffsetVector();

	if (camera->GetCamera()->GetZFar() == 0.0f)
	{
		camera->GetCamera()->SetPlaneDistance(camera->GetCamera()->GetZNear(), 100.0f);
	}
	if (camera->GetCamera()->GetZNear() == 0.0f)
	{
		camera->GetCamera()->SetPlaneDistance(0.1f, camera->GetCamera()->GetZFar());
	}
}

void CameraControllerScript::PreUpdate(float deltaTime)
{
	if (!stopped)
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
	}

	for (Component* components : owner->GetComponents())
	{
		components->OnTransformChanged();
	}

}

void CameraControllerScript::ToggleCameraState()
{
	stopped = !stopped;
}

void CameraControllerScript::ChangeCurrentPlayer(ComponentTransform* currentPlayer) 
{
	playerTransform = currentPlayer;
	stopped = false;
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

	if (App->GetModule<ModulePlayer>()->IsInCombat() && closestCombatSample)
	{
		return closestCombatSample;
	}
	else
	{
		return closestSample;
	}

}
