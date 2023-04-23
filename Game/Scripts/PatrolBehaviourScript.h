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
	void Update(float deltaTime) override;

	GameObject* GetWayPointOne() const;
	void SetWayPointOne(GameObject* wayPointOne);

	GameObject* GetWayPointTwo() const;
	void SetWayPointTwo(GameObject* wayPointTwo);


private:
	GameObject* wayPointOne;
	GameObject* wayPointTwo;

	//ComponentTransform* wayPointOneTransform;
	//ComponentTransform* wayPointTwoTransform;
	//ComponentTransform* ownerTransform;
	//ComponentRigidBody* ownerRigidBody;
};