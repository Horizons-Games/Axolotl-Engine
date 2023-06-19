#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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

	//Provisional
	GameObject* ray1GO;
	GameObject* ray2GO;
	GameObject* ray3GO;
	GameObject* ray4GO;
	ComponentTransform* ray1Transform;
	ComponentTransform* ray2Transform;
	ComponentTransform* ray3Transform;
	ComponentTransform* ray4Transform;
	//--Provisional
};
