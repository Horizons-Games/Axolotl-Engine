#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;

class CantinaNPCAnimationManager : public Script
{
public:
	CantinaNPCAnimationManager();
	~CantinaNPCAnimationManager() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentAudioSource* audio;
	ComponentAnimation* animation;
	bool activation;
	float priority;
	bool loopAnimation;

	float timer;
};