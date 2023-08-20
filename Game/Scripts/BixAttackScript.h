#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "../Scripts/ComboManager.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class Scene;

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

	bool IsDeathTouched() const;
	void SetIsDeathTouched(bool isDeathTouch);

	bool IsAttackAvailable() const;
	bool IsPerfomingJumpAttack() const;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void UpdateEnemyDetection();

	void PerformCombos();

	void LightNormalAttack();
	void HeavyNormalAttack();
	void ThrowBasicAttack();
	void JumpNormalAttack();
	void LightFinisher();
	void HeavyFinisher();
	void JumpFinisher();

	void ResetAttackAnimations();

	void DamageEnemy(GameObject* enemyAttacked, float damageAttack);

	bool isAttacking;
	bool isJumpAttacking;
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
	float comboCountLight;
	float comboCountHeavy;
	float comboCountJump;
	float attackSoft;
	float attackHeavy;
	float bulletVelocity;

	float normalAttackDistance;
	AttackCombo attackComboPhase;
	AttackType currentAttack;

	PlayerManagerScript* playerManager;

	JumpFinisherAttack* jumpFinisherScript;
	LightFinisherAttackScript* lightFinisherScript;
	HeavyFinisherAttack* heavyFinisherAttack;
	GameObject* lightWeapon;
	GameObject* bulletPrefab;

	Scene* loadedScene;
};