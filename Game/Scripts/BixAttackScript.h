#pragma once

#include "Scripting\Script.h"

#include "Components/ComponentTransform.h"

class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;

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
	void CheckCollision();

	float attackCooldown;
	float lastAttackTime;
	float rayAttackSize;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;
	GameObject* animationGO;

	ModuleInput* input;



};