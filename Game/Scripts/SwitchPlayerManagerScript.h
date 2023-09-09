#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerJumpScript;
class PlayerMoveScript;
class CameraControllerScript;

class SwitchPlayerManagerScript : public Script
{
public:
	SwitchPlayerManagerScript();
	~SwitchPlayerManagerScript() override = default;

	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};

	const std::vector<GameObject*>& GetPlayerGameObject() const;
	void SetPlayerGameObject(const std::vector<GameObject*>& vecGO);

	void TogglePlayerScripts();

private:
	void Start() override;

	ModuleInput* input;

	GameObject* mainCamera;
	CameraControllerScript* camera;
	std::vector<GameObject*> players;

};

inline const std::vector<GameObject*>& SwitchPlayerManagerScript::GetPlayerGameObject() const
{
	return players;
}

inline void SwitchPlayerManagerScript::SetPlayerGameObject(const std::vector<GameObject*>& vecGO)
{
	this->players = vecGO;
}
