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

	void SetBulletVelocity(float nVelocity);
	void SetStunTime(float nStunTime);
	void SetEnemy(GameObject* nEnemy);
	void SetDamage(float nDamageAttack);
	void SetPauseBullet(bool isPaused);
	void ResetDefaultValues();

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;

	float velocity;
	float stunTime;
	float damageAttack;
	float maxDistanceBullet;
	float maxLifeTimeBullet;
	float currentLifeTimeBullet;

	float3 defaultTargetPos;

	GameObject* enemy;
	ComponentAudioSource* audioSource;
	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	ComponentTransform* bulletTransform;
	ComponentTransform* initPos;
	ComponentParticleSystem* particleSystem;
	PlayerAttackScript* playerAttackScript;

	float particleSystemTimer;
	float particleSystemCurrentTimer;
	bool triggerParticleSystemTimer;
	bool isPaused;
};
