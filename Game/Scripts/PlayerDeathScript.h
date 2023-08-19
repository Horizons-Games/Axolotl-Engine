#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;

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
	ComponentAudioSource* componentAudioSource;
	std::string loseSceneName;
};