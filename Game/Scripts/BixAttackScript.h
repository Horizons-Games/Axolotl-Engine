#pragma once

#include "Scripting\Script.h"

class ComponentAudioSource;
class ComponentMeshCollider;
class ModuleInput;
class GameObject;

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

	GameObject* attackColliderGO;

	ComponentAudioSource* audioSource;
	ComponentMeshCollider* attackCollider;

	ModuleInput* input;



};