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

class PlayerAttackScript : public Script
{

public:
	PlayerAttackScript();
	~PlayerAttackScript() override = default;

	bool IsDeathTouched() const;
	void SetIsDeathTouched(bool isDeathTouch);

	bool IsAttackAvailable() const;
	bool IsMeleeAvailable() const;
	bool IsPerfomingJumpAttack() const;

	AttackType GetCurrentAttackType() const;
	GameObject* GetEnemyDetected() const;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void UpdateEnemyDetection();

	void PerformCombos();

	void LightNormalAttack();
	void HeavyNormalAttack();
	void JumpNormalAttack();
	void LightFinisher();
	void HeavyFinisher();
	void JumpFinisher();

	void ResetAttackAnimations();

	void DamageEnemy(GameObject* enemyAttacked, float damageAttack);
	void ThrowBasicAttack(GameObject* enemyAttacked, float nDamage);

	bool isAttacking;
	bool isMelee;
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

	float normalAttackDistance;
	AttackCombo attackComboPhase;
	AttackType currentAttack;
	AttackType lastAttack;

	PlayerManagerScript* playerManager;

	JumpFinisherAttack* jumpFinisherScript;
	LightFinisherAttackScript* lightFinisherScript;
	HeavyFinisherAttack* heavyFinisherAttack;
	GameObject* lightWeapon;
	GameObject* bulletPrefab;
	GameObject* pistolGameObject;

	Scene* loadedScene;
};