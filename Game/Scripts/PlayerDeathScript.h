#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;

class PlayerDeathScript : public Script
{
public:
	PlayerDeathScript();
	~PlayerDeathScript() override = default;

	void Start() override;

	void ManagePlayerDeath() const;

private:
	void DisablePlayerActions() const;

	ComponentAnimation* componentAnimation;
	std::string loseSceneName;
};