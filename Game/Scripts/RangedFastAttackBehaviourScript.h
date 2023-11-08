#pragma once

#include "Scripting\Script.h"
#include "..\Scripts\EnemyClass.h"

class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class Scene;
class ComponentParticleSystem;
class ComponentAgent;
class ComponentRigidBody;
class AIMovement;

class RangedFastAttackBehaviourScript : public Script
{
public:
	RangedFastAttackBehaviourScript();
	~RangedFastAttackBehaviourScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void StartAttack();
	void PerformAttack();
	void Reposition(float3 nextPosition);
	void InterruptAttack();

	bool IsAttackAvailable() const;
	bool NeedReposition() const;
	bool IsPreShooting() const;
	bool IsConsecutiveShooting() const;
	bool MovingToNewReposition();

	void ResetScriptValues();

private:
	void ShootBullet();
	GameObject* SelectBullet() const;

	float attackCooldown;
	float lastAttackTime;

	bool needReposition;
	bool movingToNewReposition;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	AIMovement* aiMovement;
	ComponentRigidBody* rb;
	ComponentParticleSystem* particleSystemShot;
	ComponentParticleSystem* particleSystemPreShot;
	ComponentTransform* particleTransform;
	ComponentTransform* particlePreShotTransform;
	ComponentTransform* shootPosition;

	GameObject* bulletLoader;
	float bulletVelocity;
	float attackDamage;

	float preShotDuration;
	bool isPreShooting;
	float preShootingTime;

	float numConsecutiveShots;
	float currentConsecutiveShots;
	float minTimeConsecutiveShot;
	float maxTimeConsecutiveShot;
	float nextShotDuration;
	float shotTime;
	bool isWaitingForConsecutiveShot;
	bool isConsecutiveShooting;

	Scene* loadedScene;

	float3 newReposition;

	EnemyTypes enemyType;
};