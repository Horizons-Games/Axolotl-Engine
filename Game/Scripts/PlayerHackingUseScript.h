#pragma once

#include "Scripting\Script.h"

#include "HackingCommand.h"
#include "ModuleInput.h"

class ComponentTransform;
class ComponentRigidBody;

class HackZoneScript;
class UIHackingManager;
class PlayerManagerScript;

class PlayerHackingUseScript : public Script
{
public:

	PlayerHackingUseScript();
	~PlayerHackingUseScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool IsHackingActive() const;

	void PrintCombination(); // DEBUG METHOD

private:

	void DisableAllInteractions();
	void EnableAllInteractions();
	void FindHackZone(const std::string& tag);
	void InitHack();
	void FinishHack();
	void RestartHack();

private:

	bool isHackingButtonPressed;
	bool isHackingActive;
	std::string hackingTag;
	HackZoneScript* hackZone;
	UIHackingManager* hackingManager;
	PlayerManagerScript* playerManager;

	KeyState keyState;

	float currentTime;
	float maxHackTime;

	ModuleInput* input;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;

	std::vector<HackingCommandType> userCommandInputs;
	std::vector<HackingCommandType> commandCombination;
};

inline bool PlayerHackingUseScript::IsHackingActive() const
{
	return isHackingActive;
}
