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

private:
	bool enableIMG = false;

	float3 startPosition;
	float3 endPosition;
	float3 resultPositon;

	ComponentTransform2D* imageTransform;
};











