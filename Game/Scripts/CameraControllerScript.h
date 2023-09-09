#pragma once

#include "Scripting/Script.h"
#include "Components/ComponentTransform.h"
class ComponentCameraSample;

class CameraControllerScript :
    public Script
{
public:
	CameraControllerScript();

	void Start() override;
    void PreUpdate(float deltaTime) override;

	void SetVerticalSensitivity(float newVerticalSensibility);
	void SetInCombat(bool newCombat);
	bool IsInCombat();

	void ChangeCurrentPlayer();
	const std::vector<GameObject*>& GetPlayerGameObject() const;
	void SetPlayerGameObject(const std::vector<GameObject*>& vecGO);

private:
	void CalculateOffsetVector();
	void CalculateOffsetVector(float3 offset);

	void CalculateFocusOffsetVector();
	void CalculateFocusOffsetVector(float2 offset);
	ComponentCameraSample* FindClosestSample(float3 position);

private: 
	int currentPlayerIndex;

	float3 finalTargetPosition;
	Quat finalTargetOrientation;

	float3 defaultOffsetVector;
	float3 defaultFocusOffsetVector;

	GameObject* samplePointsObject;
	std::vector<ComponentCameraSample*> samples;

	ComponentTransform* transform;
	ComponentCamera* camera;

	std::vector<GameObject*> players;
	ComponentTransform* playerTransform;

	float xOffset;
	float yOffset;
	float zOffset;

	float xFocusOffset;
	float yFocusOffset;

	bool inCombat;
};

inline void CameraControllerScript::SetInCombat(bool newmode)
{
	inCombat = newmode;
}

inline bool CameraControllerScript::IsInCombat()
{
	return inCombat;
}

inline const std::vector<GameObject*>& CameraControllerScript::GetPlayerGameObject() const
{
	return players;
}

inline void CameraControllerScript::SetPlayerGameObject(const std::vector<GameObject*>& vecGO)
{
	this->players = vecGO;
}
