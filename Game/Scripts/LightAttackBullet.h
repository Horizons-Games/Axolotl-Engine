#pragma once
#include "Script.h"

class ComponentTransform;
class ComponentAudioSource;
class ComponentRigidBody;
class ComponentParticleSystem;
class PlayerAttackScript;

class LightAttackBullet : public Script
{
public:
	LightAttackBullet();
	~LightAttackBullet() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetStunTime(float nStunTime);
	void SetEnemy(GameObject* nEnemy);
	void SetDamage(float nDamageAttack);
	void SetVelocity(float nVelocity);
	void SetInitPos(ComponentTransform* nInitPos);
	void StartMoving();
	void RepositionBullet();
	void SetPauseBullet(bool isPaused);
	void ResetDefaultValues();

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;

	void DestroyBullet();

	float velocity;
	float stunTime;
	float damageAttack;
	float maxLifeTimeBullet;
	float currentLifeTimeBullet;
	float lifeTime;

	GameObject* enemy;
	ComponentAudioSource* audioSource;
	ComponentRigidBody* rigidBody;
	ComponentTransform* bulletTransform;
	ComponentTransform* parentTransform;
	ComponentTransform* initPos;
	
	ComponentTransform* targetTransform;
	ComponentParticleSystem* particleSystem;
	PlayerAttackScript* playerAttackScript;

	float particleSystemTimer;
	float particleSystemCurrentTimer;
	bool triggerParticleSystemTimer;
	bool isPaused;
};
