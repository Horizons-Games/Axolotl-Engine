#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;

class CantinaNPCAnimationManager : public Script
{
public:
	CantinaNPCAnimationManager();
	~CantinaNPCAnimationManager() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentAnimation* animation;
	bool activation;
	bool loopAnimation;
	int percentageOfActivation;
};