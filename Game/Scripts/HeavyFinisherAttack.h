#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;
class ComponentAudioSource;
class ComponentTransform;
class ComponentRigidBody;
class ModulePhysics;
class GameObject;
class Scene;

class HeavyFinisherAttack : public Script
{

public:
	HeavyFinisherAttack();
	~HeavyFinisherAttack() override = default;

	void PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void SeekNextEnemy();
	void ResetValues();

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	GameObject* mesh;
	ComponentRigidBody* rigidBody;
	GameObject* player;
	ComponentTransform* playerTransform;

	ComponentTransform* target;
	ComponentTransform* attackOwner;
	bool isActivated;
	bool isReturningToOwner;
	std::vector<ComponentTransform*> enemiesInTheArea;
	std::vector<ComponentTransform*> enemiesAlreadyHit;

	ModulePhysics* physics;
	Scene* loadedScene;

	bool returnToPlayer;
	bool rotateWhileAttacking;
	float rotationVelocity;
	float damage;
	float speed;
	float hitDistance;
};
