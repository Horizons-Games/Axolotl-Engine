#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentButton;
class ComponentImage;
class ComponentTransform2D;

class UIImageControl : public Script
{
public:
	UIImageControl();
	~UIImageControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:

	float time = 0;
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float3 startPosition;
	float3 endPosition;
	float3 resultPositon;

	GameObject* trigger;
	ComponentImage* imageComponent;
	ComponentTransform2D* imageTransform;
};











