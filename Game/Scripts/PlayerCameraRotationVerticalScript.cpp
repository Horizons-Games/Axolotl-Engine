#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "../Scripts/CameraSample.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), 
		samplePointsObject(nullptr), rotationSensitivity(1.0f), transform(nullptr), player(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
	REGISTER_FIELD(player, GameObject*);
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
	playerTransform = player->GetComponent<ComponentTransform>();

	finalTargetPosition = transform->GetGlobalPosition();
	defaultOffsetVector = transform->GetGlobalPosition() - playerTransform->GetGlobalPosition();
	defaultOffset = defaultOffsetVector.Length();

	finalTargetOrientation = transform->GetGlobalRotation();

	isInfluenced = false;
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	float3 vectorOffset = defaultOffsetVector;
	Quat orientationOffset = playerTransform->GetGlobalRotation();
	CameraSample* closestSample = FindClosestSample(transform->GetGlobalPosition());
	if (closestSample && 
		(closestSample->position - playerTransform->GetGlobalPosition()).Length() <= closestSample->influenceRadius)
	{
		vectorOffset = (closestSample->position +
			closestSample->positionOffset.Normalized() * closestSample->influenceRadius * 0.9) - playerTransform->GetGlobalPosition();

		float3 eulerAngles = closestSample->orientationOffset;
		orientationOffset = Quat::FromEulerXYZ(DegToRad(eulerAngles.x), DegToRad(eulerAngles.y), DegToRad(eulerAngles.z));

		/*if (!isInfluenced)
		{
			isInfluenced = true;
		}*/
	}
	/*else
	{
		if (isInfluenced)
		{
			finalTargetPosition = playerTransform->GetGlobalPosition() + defaultOffsetVector;
			finalTargetOrientation = playerTransform->GetGlobalRotation();
			isInfluenced = false;
		}
	}*/

	finalTargetPosition = playerTransform->GetGlobalPosition() + vectorOffset;
	finalTargetOrientation = orientationOffset;

	transform->SetGlobalPosition(finalTargetPosition);
	transform->SetGlobalRotation(finalTargetOrientation);

	transform->RecalculateLocalMatrix();
}

void PlayerCameraRotationVerticalScript::Orbit(float deltaTime)
{
	Quat currentRotation = transform->GetGlobalRotation();
	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * deltaTime * rotationSensitivity;
	float verticalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().y * deltaTime * rotationSensitivity;

	Quat rotationError = Quat::identity;

	if (math::Abs(horizontalMotion) > 0.001)
	{
		rotationError = Quat::RotateY(-horizontalMotion) * rotationError;
	}

	if (math::Abs(verticalMotion) > 0.001)
	{
		float3 eulerAngles = currentRotation.ToEulerXYZ();
		float finalPitch = eulerAngles.x - verticalMotion;
		if (RadToDeg(finalPitch) < 40.0f && (RadToDeg(finalPitch) > -15.0f))
		{
			rotationError = Quat::RotateX(-verticalMotion) * rotationError;
		}
	}

	finalTargetOrientation = rotationError * currentRotation;

	float3 cameraVector = (transform->GetGlobalPosition() - playerTransform->GetGlobalPosition()).Normalized();
	finalTargetPosition = playerTransform->GetGlobalPosition() + rotationError.Transform(cameraVector) * defaultOffset;
	
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
