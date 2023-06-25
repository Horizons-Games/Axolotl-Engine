#include "CameraControllerScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCameraSample.h"

REGISTERCLASS(CameraControllerScript);

CameraControllerScript::CameraControllerScript() : Script(),
		samplePointsObject(nullptr), transform(nullptr), player(nullptr)
{
	REGISTER_FIELD(samplePointsObject, GameObject*);
	REGISTER_FIELD(player, GameObject*);
	REGISTER_FIELD(xOffset, float);
	REGISTER_FIELD(yOffset, float);
	REGISTER_FIELD(zOffset, float);
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
}

void CameraControllerScript::PreUpdate(float deltaTime)
{
	float3 sourceDirection = transform->GetGlobalForward().Normalized();
	float3 targetDirection = (playerTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
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

	ComponentCameraSample* closestSample = FindClosestSample(playerTransform->GetGlobalPosition());
	if (closestSample && 
		(closestSample->GetPosition() - playerTransform->GetGlobalPosition()).Length() <= closestSample->GetRadius())
	{
		CalculateOffsetVector(closestSample->GetOffset());

		/*float3 eulerAngles = closestSample->orientationOffset;
		orientationOffset = Quat::FromEulerXYZ(DegToRad(eulerAngles.x), DegToRad(eulerAngles.y), DegToRad(eulerAngles.z));*/

	}
	else
	{
		CalculateOffsetVector();
	}

	finalTargetPosition = playerTransform->GetGlobalPosition() + defaultOffsetVector;
	finalTargetOrientation = orientationOffset;

	transform->SetGlobalPosition(finalTargetPosition);
	transform->SetGlobalRotation(finalTargetOrientation);
	transform->RecalculateLocalMatrix();
}

void CameraControllerScript::CalculateOffsetVector()
{
	//defaultOffsetVector = -float3::unitZ * zOffset + playerTransform->GetGlobalUp().Normalized() * yOffset;
	defaultOffsetVector = float3::unitX * xOffset + float3::unitY * yOffset + float3::unitZ * zOffset;
	defaultOffset = defaultOffsetVector.Length();
}

void CameraControllerScript::CalculateOffsetVector(float3 offset)
{

	defaultOffsetVector = offset;
	defaultOffset = defaultOffsetVector.Length();
}


ComponentCameraSample* CameraControllerScript::FindClosestSample(float3 position)
{
	ComponentCameraSample* closestSample = nullptr;
	float minDistance = std::numeric_limits<float>::max();

	for (auto sample : samples)
	{
		float distance = (sample->GetPosition() - position).Length();
		if (distance < minDistance)
		{
			closestSample = sample;
			minDistance = distance;
		}
	}
	return closestSample;
}
