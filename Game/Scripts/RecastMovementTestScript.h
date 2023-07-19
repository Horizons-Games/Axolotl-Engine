#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAgent;
class ComponentTransform;
class ModuleInput;

class RecastMovementTestScript : public Script
{
public:
	RecastMovementTestScript();
	~RecastMovementTestScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void Move();

private:
	float3 targetPosition;
	bool test = true;

	ComponentAgent* agent;
	ComponentTransform* componentTransform;
	GameObject* target;

	ModuleInput* input;
};