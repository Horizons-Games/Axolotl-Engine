#pragma once
#include "Script.h"

class ComponentRigidBody;
class ComponentTransform;
class ComponentAudioSource;

class LightAttackBullet : public Script
{
public:
	LightAttackBullet();
	~LightAttackBullet() override = default;

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
