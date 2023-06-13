#pragma once

#include "Scripting\Script.h"


class ComponentTransform;
class ComponentAudioSource;
class Scene;

class DroneBullet : public Script
{

public:
	DroneBullet();
	~DroneBullet() override = default;

private:
	void Start() override;
	void Update(float deltaTime) override;

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