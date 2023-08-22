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

	bool isHackingActive;

	std::string hackingTag;

	ModuleInput* input;
};

inline bool PlayerHackingUseScript::IsHackingActive() const
{
	return isHackingActive;
}
