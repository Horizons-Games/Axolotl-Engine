#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "../Scripts/ComboManager.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;

class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;
class EntityDetection;
class ComboManager;
class HeavyFinisherAttack;

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

	void UpdateEnemyDetection();

	void NormalAttack();
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
	float normalAttackDistance;
	AttackType currentAttack;

	HeavyFinisherAttack* heavyFinisherAttack;
	GameObject* bixLightSaber;

	PlayerManagerScript* playerManager;

	AttackCombo attackComboPhase;
};