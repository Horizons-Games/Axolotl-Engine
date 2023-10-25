#pragma once

#include "Scripting/Script.h"
#include "Components/ComponentTransform.h"
class ComponentCameraSample;

class CameraBossControllerScript :
    public Script
{
public:
	CameraBossControllerScript();

	void Start() override;
    void PreUpdate(float deltaTime) override;

	void SetInCombat(bool newCombat);
	bool IsInCombat();

	void ToggleCameraState();
	void ChangeCurrentPlayer(ComponentTransform* currentPlayer);

private:
	void CalculateOffsetVector();
	void CalculateOffsetVector(float3 offset);

	void CalculateFocusOffsetVector();
	void CalculateFocusOffsetVector(float2 offset);
	void CalculateCameraPositionByBoss();

private: 

	float3 finalTargetPosition;
	Quat finalTargetOrientation;

	float3 defaultOffsetVector;
	float3 defaultFocusOffsetVector;

	ComponentTransform* transform;
	ComponentCamera* camera;

	GameObject* player;
	ComponentTransform* playerTransform;

	GameObject* boss;
	ComponentTransform* bossTransform;

	float xOffset;
	float yOffset;
	float zOffset;

	float xFocusOffset;
	float yFocusOffset;
	float yFocusOffsetBossUp;
	float actualYFocusOffset;

	float minDistance;
	float minMultiplier;
	float maxDistance;
	float maxMultiplier;
	float HeightDistance;
	float Heightmultiplier;
	float multiplierWithHeight;

	bool stopped;
};