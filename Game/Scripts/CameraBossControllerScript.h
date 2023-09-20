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

	void SetVerticalSensitivity(float newVerticalSensibility);
	void SetInCombat(bool newCombat);
	bool IsInCombat();

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

	bool inCombat;
};

inline void CameraBossControllerScript::SetInCombat(bool newmode)
{
	inCombat = newmode;
}

inline bool CameraBossControllerScript::IsInCombat()
{
	return inCombat;
}
