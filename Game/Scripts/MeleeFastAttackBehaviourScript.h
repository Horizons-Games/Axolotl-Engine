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

	void PerformAttack(GameObject* target);
	bool IsAttackAvailable();

	void ResetScriptValues();

private:

	float attackCooldown;
	float lastAttackTime;
	float damageAttack;

	ComponentTransform* ownerTransform;

	ComponentAudioSource* audioSource;
};