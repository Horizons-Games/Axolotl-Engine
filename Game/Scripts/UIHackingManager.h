#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

#include "HackingCommand.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class UIHackingManager : public Script
{
public:
	UIHackingManager();
	~UIHackingManager() override = default;

	void Init() override;

	void AddInputVisuals(HackingCommandType type);
	void RemoveInputVisuals();

	void CleanInputVisuals();

	void EnableHackingTimer();
	void DisableHackingTimer();

	void SetHackingTimerValue(float maxHackTime, float currentTime);
private:

	GameObject* command_A;
	GameObject* command_X;
	GameObject* command_Y;
	GameObject* hackingTimer;

	std::deque<GameObject*> inputVisuals;
	std::vector<GameObject*> inputPositions;
};