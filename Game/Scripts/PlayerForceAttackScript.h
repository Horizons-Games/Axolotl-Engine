#pragma once
#include "Script.h"
class PlayerForceAttackScript : public Script
{
public:
	PlayerForceAttackScript();
	~PlayerForceAttackScript();

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;
	virtual void OnCollisionExit(ComponentRigidBody* other) override;

	void PushEnemies();

private:
	std::vector<GameObject*> enemiesInTheArea;

	float force;
	float stunTime;
	float coolDown;
	float currentCoolDown;
};
