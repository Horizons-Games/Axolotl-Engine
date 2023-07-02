#pragma once

#include "Scripting\Script.h"

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

private:
	void ShootBullet(float deltaTime);
	void CheckCollision();
	void DestroyBullet();

	float velocity;
	float bulletLifeTime;
	float damageAttack;
	float rayAttackSize;
	float originTime;

	ComponentTransform* transform;
	ComponentAudioSource* audioSource;
};