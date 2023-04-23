#pragma once

#include "Scripting\Script.h"

// This script performs what it was recommended that a basic script should do in the Gameplay Systems PPT

class ComponentTransform;
class ComponentRigidBody;

class PatrolBehaviourScript : public Script
{
public:
	PatrolBehaviourScript();
	~PatrolBehaviourScript() override = default;

	void Init() override {};
	void Start() override;
	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

	GameObject* GetWayPointOne() const;
	void SetWayPointOne(GameObject* wayPointOne);

	GameObject* GetWayPointTwo() const;
	void SetWayPointTwo(GameObject* wayPointTwo);


private:
	GameObject* wayPointOne;
	GameObject* wayPointTwo;

	//ComponentTransform* wayPointOneTransform;
	//ComponentTransform* wayPointTwoTransform;
	//ComponentRigidBody* ownerRigidBody;
};