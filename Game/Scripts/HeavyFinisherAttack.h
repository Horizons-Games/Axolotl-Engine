#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;
class ComponentAudioSource;
class ComponentTransform;
class ComponentRigidBody;
class ComponentParticleSystem;
class PlayerAttackScript;
class ModulePhysics;
class GameObject;
class Scene;

class HeavyFinisherAttack : public Script
{

public:
	HeavyFinisherAttack();
	~HeavyFinisherAttack() override = default;

	void PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner);
	void PerformEmptyHeavyFinisher(ComponentTransform* attackOwner);
	bool IsAttacking();

private:
	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	void MoveToEnemy(float deltaTime);
	void MoveForward(float deltaTime);
	void SeekNextEnemy();
	void HitEnemy();
	void ResetValues();

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	GameObject* mesh;
	ComponentRigidBody* rigidBody;
	ComponentParticleSystem* vfx;

	PlayerAttackScript* playerAttackScript;

	ComponentTransform* target;
	ComponentTransform* attackOwner;
	bool isActivated;
	bool isReturningToOwner;
	bool attackHasTarget;
	std::vector<ComponentTransform*> enemiesInTheArea;
	std::vector<ComponentTransform*> enemiesAlreadyHit;
	float3 emptyAttackTargetPos;

	ModulePhysics* physics;
	Scene* loadedScene;

	bool returnToPlayer;
	bool rotateWhileAttacking;
	float rotationVelocity;
	float damage;
	float speed;
	float hitDistance;
	float maxEnemyHits; //Should be int but can't REGISTER_FIELD ints
	float defaultThrowDistance; //Distance for when the attack does not have an initial target
};
