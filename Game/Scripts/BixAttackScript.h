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
class PlayerManagerScript;
class JumpFinisherAttack;
class LightFinisherAttackScript;
class HeavyFinisherAttack;

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

	bool GetIsDeathTouched() const;
	void SetIsDeathTouched(bool isDeathTouch);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void UpdateEnemyDetection();

	void NormalAttack();
	void NormalJumpAttack();
	void LightFinisher();
	void HeavyFinisher();
	void JumpFinisher();

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
	AttackCombo attackComboPhase;
	AttackType currentAttack;

	PlayerManagerScript* playerManager;

	JumpFinisherAttack* jumpFinisherScript;
	LightFinisherAttackScript* lightFinisherScript;
	HeavyFinisherAttack* heavyFinisherAttack;
	GameObject* bixLightSaber;
};
