#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "../Scripts/CameraSample.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), 
		samplePointsObject(nullptr), rotationSensitivity(1.0f), transform(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
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

	finalTargetPosition = transform->GetGlobalPosition();
	defaultOffset = (transform->GetGlobalPosition() - parentTransform->GetGlobalPosition()).Length();

	finalTargetOrientation = transform->GetGlobalRotation();

	isInfluenced = false;
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	CameraSample* closestSample = FindClosestSample(transform->GetGlobalPosition());
	if (closestSample && 
		(closestSample->position - parentTransform->GetGlobalPosition()).Length() <= closestSample->influenceRadius)
	{
		finalTargetPosition = closestSample->position + 
			closestSample->positionOffset.Normalized()* closestSample->influenceRadius*0.9;

		float3 eulerAngles = closestSample->orientationOffset;
		finalTargetOrientation = Quat::FromEulerXYZ(DegToRad(eulerAngles.x), DegToRad(eulerAngles.y), DegToRad(eulerAngles.z));

		if (!isInfluenced)
		{
			isInfluenced = true;
		}
	}
	else
	{
		if (isInfluenced)
		{
			finalTargetPosition = parentTransform->GetGlobalPosition() - parentTransform->GetGlobalForward()*defaultOffset;
			finalTargetOrientation = parentTransform->GetGlobalRotation();
			isInfluenced = false;
		}
		else
		{
			Orbit(deltaTime);
		}
	}

	transform->SetGlobalPosition(finalTargetPosition);
	transform->SetGlobalRotation(finalTargetOrientation);

	transform->RecalculateLocalMatrix();
}

void PlayerCameraRotationVerticalScript::Orbit(float deltaTime)
{
	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * deltaTime * rotationSensitivity;
	Quat rotationError = Quat::identity;
	if (math::Abs(horizontalMotion) > 0.001)
	{
		rotationError = Quat::RotateY(-horizontalMotion);
	}

	Quat currentRotation = transform->GetGlobalRotation();
	finalTargetOrientation = rotationError * currentRotation;

	float3 cameraVector = (transform->GetGlobalPosition() - parentTransform->GetGlobalPosition()).Normalized();
	finalTargetPosition = parentTransform->GetGlobalPosition() + rotationError.Transform(cameraVector) * defaultOffset;
	
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
