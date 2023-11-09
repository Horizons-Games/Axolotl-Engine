#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentAudioSource;

enum class FireState
{
	GROUP1_ACTIVE,
	GROUP2_ACTIVE,
	GROUP1_WAITING_TO_ACTIVATE,
	GROUP2_WAITING_TO_ACTIVATE
};

class MultipleFireObstacle : public Script
{
public:
	MultipleFireObstacle();
	~MultipleFireObstacle() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	void CollisionDetected(ComponentRigidBody* other);

	std::vector<GameObject*> fireGroup1;
	std::vector<GameObject*> fireGroup2;

	float activatedTime;
	float deactivatedTime;

	float timer;
	FireState fireState;

	ComponentAudioSource* audioSource;
};


