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
	bool IsMovingToEnd() const;
	bool IsMoving() const;
	void SetIsMoving(bool isMoving);
	bool SetMovingToEnd(bool movingToEnd);

private:
	bool movingToEnd = false;
	bool moving = false;

	float3 startPosition;
	float3 endPosition;
	float3 resultPositon;

	ComponentTransform2D* imageTransform;
};

inline bool UIImageDisplacementControl::IsMovingToEnd() const
{
	return movingToEnd;
}

inline bool UIImageDisplacementControl::SetMovingToEnd(bool movingToEnd)
{
	return movingToEnd = movingToEnd;
}

inline bool UIImageDisplacementControl::IsMoving() const
{
	return moving;
}

inline void UIImageDisplacementControl::SetIsMoving(bool IsMoving) 
{

	this->moving = IsMoving;

}











