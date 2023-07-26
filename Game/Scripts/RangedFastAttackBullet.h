#pragma once

#include "Scripting\Script.h"

class ComponentRigidBody;
class ComponentTransform;
class ComponentAudioSource;
class Scene;

class RangedFastAttackBullet : public Script
{
public:
	RangedFastAttackBullet();
	~RangedFastAttackBullet() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetBulletVelocity(float velocity);

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void DestroyBullet();

	float velocity;
	float bulletLifeTime;
	float damageAttack;
	float rayAttackSize;
	float originTime;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	ComponentAudioSource* audioSource;
};