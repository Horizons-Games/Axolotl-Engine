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

private:

	void DisableAllInteractions();
	void EnableAllInteractions();
	void FindHackZone(const std::string& tag);

private:

	bool isHackingActive;
	std::string hackingTag;
	HackZoneScript* hackZone;

	float currentTime;
	float maxHackTime;

	ModuleInput* input;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;
};

inline bool PlayerHackingUseScript::IsHackingActive() const
{
	return isHackingActive;
}
