#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentAudioSource;

class MeleeFastAttackBehaviourScript : public Script
{
public:
	MeleeFastAttackBehaviourScript();
	~MeleeFastAttackBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	bool IsAttackAvailable();

	void ResetScriptValues();

private:
	void CheckCollision();

	float attackCooldown;
	float lastAttackTime;
	float damageAttack;

	//Provisional
	std::vector<Ray> rays;

	GameObject* ray1GO;
	GameObject* ray2GO;
	GameObject* ray3GO;
	GameObject* ray4GO;

	ComponentTransform* ray1Transform;
	ComponentTransform* ray2Transform;
	ComponentTransform* ray3Transform;
	ComponentTransform* ray4Transform;

	float rayAttackSize;
	//--Provisional

	ComponentTransform* ownerTransform;

	ComponentAudioSource* audioSource;
};