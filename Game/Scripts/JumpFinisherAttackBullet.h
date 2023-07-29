#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class JumpFinisherArea;

class JumpFinisherAttackBullet : public Script
{
public:
	JumpFinisherAttackBullet();
	~JumpFinisherAttackBullet() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;

	void SetBulletVelocity(float velocity);

	void SetForceArea(JumpFinisherArea* newForceArea);
	JumpFinisherArea* GetForceArea() const;

private:
	void InitializeBullet();
	void DestroyBullet() const;

	ComponentTransform* parentTransform;
	ComponentRigidBody* rigidBody;

	JumpFinisherArea* forceArea;

	float bulletVelocity;
	float bulletHeightForce;
	float originTime;
	float bulletLifeTime;
};