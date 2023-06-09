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

	void IncreaseAttack(float ammountAttack);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	bool isAttackAvailable();
	void CheckCollision();

	float attackCooldown;
	float lastAttackTime;
	float rayAttackSize;
	float damageAttack;

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

inline void BixAttackScript::IncreaseAttack(float ammountAttack)
{
	damageAttack += ammountAttack;
	ENGINE_LOG("Attack increased");
}
