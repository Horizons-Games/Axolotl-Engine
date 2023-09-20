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
	REGISTER_FIELD(inCombat, bool);
}

void CameraBossControllerScript::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	camera = GetOwner()->GetComponentInternal<ComponentCamera>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = player->GetComponent<ComponentTransform>();
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

	if (boss)
	{
		CalculateCameraPositionByBoss();
		CalculateFocusOffsetVector();
		camera->RestoreKpPosition();
		camera->RestoreKpRotation();
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
		+ float3::unitY * yFocusOffset;
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
	float3 vector = (playerPos - bossPos).Normalized();

	float distance = playerPos.Distance(bossPos);
	float multiplier = Min(Max(distance / 50, 0.5f),1.f);

	float3 offset = float3(vector.x * multiplier *10, yOffset, vector.z * multiplier * 10);
	LOG_DEBUG("offset: {}, {}, {}", offset.x, offset.y, offset.z);
	CalculateOffsetVector(offset);
}