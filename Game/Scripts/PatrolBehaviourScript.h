#pragma once

#include "Scripting\Script.h"

// This script performs a generic patrol behaviour between two (or more) waypoints

class ComponentTransform;
class ComponentRigidBody;

class PatrolBehaviourScript : public Script
{
public:
	PatrolBehaviourScript();
	~PatrolBehaviourScript() override = default;

	void Start() override;

	void StartPatrol() const;
	void Patrolling();

private:
	GameObject* wayPointOne;
	GameObject* wayPointTwo;

	ComponentTransform* wayPointOneTransform;
	ComponentTransform* wayPointTwoTransform;
	ComponentTransform* currentWayPointTransform;
	ComponentTransform* ownerTransform;
	ComponentRigidBody* ownerRigidBody;
	GameObject* ownerRigidBodyGO;
};