#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;

class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;
class EntityDetection;
class ComboManager;

class EntityDetection;
class PlayerManagerScript;

enum class AttackCombo
{
	IDLE,
	FIRST_ATTACK,
	SECOND_ATTACK,
	THIRD_ATTACK
};

class BixAttackScript : public Script
{

public:
	BixAttackScript();
	~BixAttackScript() override = default;

	bool IsDeathTouched() const;
	void SetIsDeathTouched(bool isDeathTouch);
	bool IsAttacking() const;
	void SetIsAttacking(bool isAttacking);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void NormalAttack(bool heavy);
	void JumpAttack();
	void SoftFinisher();
	void HeavyFinisher();

	void DamageEnemy(GameObject* enemyAttacked, float damageAttack);
	void ActivateAnimationCombo();

	bool IsAttackAvailable() const;

	bool isAttacking;
	float attackCooldown;
	float attackCooldownCounter;
	float comboInitTimer;
	float comboNormalAttackTimer;

	bool isDeathTouched;

	ModuleInput* input;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;

	GameObject* animationGO;
	GameObject* enemyDetectionObject;

	EntityDetection* enemyDetection;
	ComboManager* comboSystem;
	float comboCountHeavy;
	float comboCountSoft;
	float attackSoft;
	float attackHeavy;

	PlayerManagerScript* playerManager;

	AttackCombo attackComboPhase;
};

inline void BixAttackScript::SetIsAttacking(bool isAttaking)
{
	this->isAttacking = isAttacking;
}

inline bool BixAttackScript::IsAttacking() const
{
	return isAttacking;
}

inline bool BixAttackScript::IsDeathTouched() const
{
	return isDeathTouched;
}

inline void BixAttackScript::SetIsDeathTouched(bool isDeathTouched)
{
	this->isDeathTouched = isDeathTouched;
}

inline bool BixAttackScript::IsAttackAvailable() const
{
	return !isAttacking;
}
