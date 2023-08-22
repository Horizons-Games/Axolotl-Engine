#pragma once

#include "Scripting\Script.h"

class ModuleInput;

class PlayerHackingUseScript : public Script
{
public:

	PlayerHackingUseScript();
	~PlayerHackingUseScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool IsHackingActive() const;

private:

	void DisableAllInteractions();
	void EnableAllInteractions();
	

private:

	bool isHackingActive;
	std::string hackingTag;
	float currentTime;
	float maxHackTime;
	ModuleInput* input;
};

inline bool PlayerHackingUseScript::IsHackingActive() const
{
	return isHackingActive;
}
