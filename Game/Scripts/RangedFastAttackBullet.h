#pragma once

#include "Scripting\Script.h"

class ComponentRigidBody;
class ComponentTransform;
class ComponentAudioSource;
class ComponentParticleSystem;
class ComponentMeshRenderer;

class RangedFastAttackBullet : public Script
{
public:
	RangedFastAttackBullet();
	~RangedFastAttackBullet() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void SetBulletVelocity(float nVelocity);
	void SetTargetTag(std::string nTag);
	void SetBulletDamage(float damage);
	void SetPauseBullet(bool isPaused);
	void SetImpactSound(const wchar_t* sound);

private:
	void InitializeBullet();
	void DestroyBullet();

	float velocity;
	float bulletLifeTime;
	float damageAttack;
	float rayAttackSize;
	float originTime;
	bool waitParticlesToDestroy;
	float particlesDuration;
	const wchar_t* impactSFX;

	std::string targetTag;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	ComponentAudioSource* audioSource;
	ComponentParticleSystem* particleSystem;
	ComponentMeshRenderer* mesh;
};