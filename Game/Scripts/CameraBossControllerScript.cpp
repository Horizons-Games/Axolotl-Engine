#include "StdAfx.h"
#include "CameraBossControllerScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCameraSample.h"

#include "Camera/CameraGameObject.h"

#include "AxoLog.h"

REGISTERCLASS(CameraBossControllerScript);

CameraBossControllerScript::CameraBossControllerScript() : Script(),
		transform(nullptr), player(nullptr), boss(nullptr)
{
	REGISTER_FIELD(boss, GameObject*);
	REGISTER_FIELD(xOffset, float);
	REGISTER_FIELD(yOffset, float);
	REGISTER_FIELD(zOffset, float);
	REGISTER_FIELD(xFocusOffset, float);
	REGISTER_FIELD(yFocusOffset, float);
	REGISTER_FIELD(yFocusOffsetBossUp, float);
	REGISTER_FIELD(minDistance, float);
	REGISTER_FIELD(minMultiplier, float);
	REGISTER_FIELD(maxDistance, float);
	REGISTER_FIELD(maxMultiplier, float);
	REGISTER_FIELD(HeightDistance, float);
	REGISTER_FIELD(Heightmultiplier, float);
	REGISTER_FIELD(multiplierWithHeight, float);

}

void CameraBossControllerScript::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	camera = GetOwner()->GetComponentInternal<ComponentCamera>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = player->GetComponent<ComponentTransform>();
	actualYFocusOffset = yFocusOffset;
	if (boss)
	{
		bossTransform = boss->GetComponent<ComponentTransform>();
	}

	finalTargetPosition = transform->GetGlobalPosition();
	finalTargetOrientation = transform->GetGlobalRotation();
	CalculateOffsetVector();
	CalculateFocusOffsetVector();
}

void CameraBossControllerScript::PreUpdate(float deltaTime)
{
	if (!stopped)
	{
		if (boss)
		{
			CalculateCameraPositionByBoss();
		}
		else
		{
			CalculateOffsetVector();
		}

		CalculateFocusOffsetVector();
		camera->RestoreKpPosition();
		camera->RestoreKpRotation();

		float3 sourceDirection = camera->GetCamera()->GetFrustum()->Front().Normalized();

		float3 directionalTargetVector = playerTransform->GetGlobalPosition()
			+ defaultFocusOffsetVector
			- camera->GetCamera()->GetPosition();
		directionalTargetVector.y = 0.0f;

		float3 targetDirection = directionalTargetVector.Normalized();

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
}

void CameraBossControllerScript::ToggleCameraState()
{
	stopped = !stopped;
}

void CameraBossControllerScript::ChangeCurrentPlayer(ComponentTransform* currentPlayer)
{
	playerTransform = currentPlayer;
	stopped = false;
}

void CameraBossControllerScript::CalculateOffsetVector()
{
	defaultOffsetVector = float3::unitX * xOffset 
		+ float3::unitY * yOffset 
		+ float3::unitZ * zOffset;
}

void CameraBossControllerScript::CalculateOffsetVector(float3 offset)
{
	defaultOffsetVector = offset;
}


void CameraBossControllerScript::CalculateFocusOffsetVector()
{
	float3 currentFocus = (playerTransform->GetGlobalPosition() - camera->GetCamera()->GetPosition()).Normalized();
	float3 rightVector = currentFocus.Cross(float3::unitY);
	defaultFocusOffsetVector = rightVector * xFocusOffset
		+ float3::unitY * actualYFocusOffset;
}

void CameraBossControllerScript::CalculateFocusOffsetVector(float2 offset)
{
	float3 currentFocus = (playerTransform->GetGlobalPosition() - camera->GetCamera()->GetPosition()).Normalized();
	float3 rightVector = currentFocus.Cross(float3::unitY);
	defaultFocusOffsetVector = rightVector * offset.x
		+ float3::unitY * offset.y;
}

void CameraBossControllerScript::CalculateCameraPositionByBoss()
{
	float3 playerPos = playerTransform->GetGlobalPosition();
	float3 bossPos = bossTransform->GetGlobalPosition();

	float2 playerPosXZ = float2(playerPos.x, playerPos.z);
	float2 bossPosXZ = float2(bossPos.x, bossPos.z);

	float2 vectorBossPlayerXZ = playerPosXZ - bossPosXZ;
	float2 vectorBossPlayerXZNorm = vectorBossPlayerXZ.Normalized();
	float newyOffset = yOffset;

	float distanceBossPlayerXZ = vectorBossPlayerXZ.Length();

	float multiplier;
	if (distanceBossPlayerXZ <= minDistance)
	{
		multiplier = minMultiplier;
	}

	else if (distanceBossPlayerXZ >= maxDistance)
	{
		multiplier = maxMultiplier;
	}
	else
	{
		float t = (distanceBossPlayerXZ - minDistance)/ (maxDistance - minDistance);
		multiplier = (1 - t) * minMultiplier + t * maxMultiplier;
	}

	if (bossPos.y > playerPos.y + HeightDistance)
	{
		newyOffset += Heightmultiplier;
		multiplier += multiplierWithHeight;
		actualYFocusOffset = yFocusOffsetBossUp;
	}
	else
	{
		actualYFocusOffset = yFocusOffset;
	}

	float3 offset = float3(vectorBossPlayerXZNorm.x * multiplier, newyOffset, vectorBossPlayerXZNorm.y * multiplier);
	CalculateOffsetVector(offset);
}