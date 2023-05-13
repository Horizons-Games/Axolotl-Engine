#pragma once

#include "Scripting\Script.h"

#include "Components/ComponentTransform.h"

class ComponentAudioSource;
class ModuleInput;

class BixAttackScript : public Script
{

public:
	BixAttackScript();
	~BixAttackScript() override = default;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	bool isAttackAvailable();

	float attackCooldown;
	float lastAttackTime;
	float rayAttackSize;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;

	ModuleInput* input;



};