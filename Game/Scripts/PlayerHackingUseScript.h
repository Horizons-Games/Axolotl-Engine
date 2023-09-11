#pragma once

#include "Scripting\Script.h"

#include "HackingCommand.h"

class ModuleInput;
class ComponentTransform;
class ComponentRigidBody;

class HackZoneScript;
class UIHackingManager;


const std::vector<HackingCommandType> allHackingCommands = { COMMAND_A, COMMAND_B, COMMAND_X, COMMAND_Y };


class PlayerHackingUseScript : public Script
{
public:

	PlayerHackingUseScript();
	~PlayerHackingUseScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool IsHackingActive() const;

	void PrintCombination(); //DEBUG METHOD

private:

	void DisableAllInteractions();
	void EnableAllInteractions();
	void FindHackZone(const std::string& tag);
	void InitHack();
	void FinishHack();

private:

	bool isHackingActive;
	std::string hackingTag;
	HackZoneScript* hackZone;
	UIHackingManager* hackingManager;

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
