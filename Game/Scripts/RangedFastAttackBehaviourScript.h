#pragma once

#include "Scripting\Script.h"

class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;
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

	float attackCooldown;
	float lastAttackTime;

	bool needReposition;
	bool movingToNewReposition;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;
	AIMovement* aiMovement;
	ComponentRigidBody* rb;
	ComponentParticleSystem* particleSystemShot;
	ComponentParticleSystem* particleSystemPreShot;
	ComponentTransform* particleTransform;
	ComponentTransform* particlePreShotTransform;
	ComponentTransform* shootPosition;

	GameObject* bulletPrefab;
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
};