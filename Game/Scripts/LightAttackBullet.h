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
	void SetImpactSound(const wchar_t* sound);

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void DestroyBullet();

	float velocity;
	float stunTime;
	float damageAttack;
	float maxDistanceBullet;
	const wchar_t* impactSFX;

	float3 defaultTargetPos;

	GameObject* enemy;
	ComponentAudioSource* audioSource;
	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	ComponentParticleSystem* particleSystem;
	PlayerAttackScript* playerAttackScript;

	float particleSystemTimer;
	float particleSystemCurrentTimer;
	bool triggerParticleSystemTimer;
	bool isPaused;
};
