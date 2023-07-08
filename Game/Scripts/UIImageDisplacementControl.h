#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform2D;

class UIImageDisplacementControl : public Script
{
public:
	UIImageDisplacementControl();
	~UIImageDisplacementControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void MoveImageToEndPosition();
	void MoveImageToStarPosition();
	bool GetIsMovingToEnd() const;
	bool GetIsMoving() const;
	bool SetMovingToEnd(bool movingToEnd);

private:
	bool isMovingToEnd = false;
	bool isMoving = false;

	float3 startPosition;
	float3 endPosition;
	float3 resultPositon;

	ComponentTransform2D* imageTransform;
};

inline bool UIImageDisplacementControl::GetIsMovingToEnd() const
{
	return isMovingToEnd;
}

inline bool UIImageDisplacementControl::SetMovingToEnd(bool movingToEnd)
{
	return isMovingToEnd = movingToEnd;
}

inline bool UIImageDisplacementControl::GetIsMoving() const
{
	return isMoving;
}











