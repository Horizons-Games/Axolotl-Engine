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

private:

	GameObject* command_A;
	GameObject* command_X;
	GameObject* command_Y;

	std::deque<GameObject*> inputVisuals;
	std::vector<GameObject*> inputPositions;
};