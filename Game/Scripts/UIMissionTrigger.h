#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

#include "UIImageDisplacementControl.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentRigidBody;
class ComponentScript;
class UIImageDisplacementControl;

class UIMissionTrigger : public Script
{

public:
	UIMissionTrigger();
	~UIMissionTrigger();

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:

	float maxTimeTextImageOn;
	float currentTime;
	bool wasInside = false;

	ComponentPlayer* player;
	ComponentRigidBody* componentRigidBody;
	GameObject* missionLevel;
	GameObject* lastMissionLevel;
	GameObject* textBox;
	UIImageDisplacementControl* missionImageDisplacement;
	UIImageDisplacementControl* missionImageDisplacementExit;
};