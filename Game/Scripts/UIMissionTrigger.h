#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

#include "UIImageDisplacementControl.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentRigidBody;
class ComponentScript;
class UIImageDisplacementControl;
class HackZoneScript;

class UIMissionTrigger : public Script
{

public:
	UIMissionTrigger();
	~UIMissionTrigger();

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	void DisableTextBox(float time);
	void ActivateTextBoxManually();

private:

	float maxTimeTextImageOn;
	float currentTime;
	bool wasInside = false;
	bool hasTimer;
	bool waitForNotInCombat;
	bool waitForSwitch;
	bool missionCondition;
	bool waitForHack;

	ComponentPlayer* player;
	ComponentRigidBody* componentRigidBody;
	GameObject* missionLevel;
	GameObject* lastMissionLevel;
	GameObject* textBox;
	GameObject* switchManager;
	UIImageDisplacementControl* missionImageDisplacement;
	UIImageDisplacementControl* missionImageDisplacementExit;
	HackZoneScript* hackZoneScript;
};
