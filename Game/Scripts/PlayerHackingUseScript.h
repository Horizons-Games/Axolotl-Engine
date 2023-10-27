#pragma once

#include "Scripting\Script.h"

#include "HackingCommand.h"
#include "ModuleInput.h"

class ComponentTransform;
class ComponentRigidBody;
class ComponentAudioSource;

class HackZoneScript;
class UIHackingManager;
class PlayerManagerScript;
class SwitchPlayerManagerScript;

class PlayerHackingUseScript : public Script
{
public:
	PlayerHackingUseScript();
	~PlayerHackingUseScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool IsInsideValidHackingZone() const;

	bool IsHackingActive() const;

	void PrintCombination(); // DEBUG METHOD
	
	void StopHackingParticles();
	
private:
	void DisableAllInteractions() const;
	void EnableAllInteractions() const;
	void FindHackZone(const std::string& tag);
	void InitHack();
	void FinishHack();
	void RestartHack();
	void CheckCurrentHackZone();

private:
	bool isHackingButtonPressed;
	bool isHackingActive;
	std::string hackingTag;
	HackZoneScript* hackZone;
	UIHackingManager* hackingManager;
	SwitchPlayerManagerScript* switchPlayerManager;
	PlayerManagerScript* playerManager;

	float currentTime;
	float maxHackTime;

	ModuleInput* input;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;
	ComponentAudioSource* audioSource;

	std::vector<HackingCommandType> userCommandInputs;
	std::vector<HackingCommandType> commandCombination;
};
