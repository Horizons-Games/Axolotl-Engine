#pragma once

#include "Scripting\Script.h"

class ModuleInput;
class ComponentTransform;
class ComponentRigidBody;

class HackZoneScript;

class PlayerHackingUseScript : public Script
{
public:

	PlayerHackingUseScript();
	~PlayerHackingUseScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool IsHackingActive() const;

	void PrintCombination(); //TEMPORARY METHOD

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

	float currentTime;
	float maxHackTime;

	ModuleInput* input;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;

	std::vector<SDL_Scancode> userKeyInputs;
	std::vector<SDL_GameControllerButton> userButtonInputs;

	std::vector<SDL_Scancode> keyCombination;
	std::vector<SDL_GameControllerButton> buttonCombination;

	int buttonIndex;
	int keyIndex;
};

inline bool PlayerHackingUseScript::IsHackingActive() const
{
	return isHackingActive;
}
