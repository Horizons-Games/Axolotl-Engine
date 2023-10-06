#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class JumpFinisherArea;
class EntityDetection;

class JumpFinisherAttackBullet : public Script
{
public:
	JumpFinisherAttackBullet();
	~JumpFinisherAttackBullet() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;

	void SetBulletVelocity(float velocity);

	void SetAreaPushForce(float newAreaPushForce);
	void SetAreaStunTime(float newAreaStunTime);

private:
	void InitializeBullet();
	void ThrowBulletToTheFloor() const;
	void DestroyBullet() const;

	ComponentTransform* parentTransform;
	ComponentRigidBody* rigidBody;

	float bulletVelocity;
	float bulletFallForce;
	float originTime;
	float bulletLifeTime;

	float areaPushForce;
	float areaStunTime;

	EntityDetection* enemyDetection;
};