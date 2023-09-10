#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Timer/Timer.h"

RUNTIME_MODIFIABLE_INCLUDE;

class CameraControllerScript;
class ComponentTransform;
class ComponentRigidBody;
class ModuleInput;

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
	void IsChangingCurrentPlayer();

private:
	void Start() override;

	ModuleInput* input;

	Timer changePlayerTimer;
	GameObject* mainCamera;
	CameraControllerScript* camera;
	std::vector<GameObject*> players;

	ComponentTransform* player0Transform;
	ComponentTransform* player1Transform;
	ComponentTransform* currentPlayerTransform;

	ComponentRigidBody* player0RigidBody;
	ComponentRigidBody* player1RigidBody;

	bool isChangingPlayer;
};

inline const std::vector<GameObject*>& SwitchPlayerManagerScript::GetPlayerGameObject() const
{
	return players;
}

inline void SwitchPlayerManagerScript::SetPlayerGameObject(const std::vector<GameObject*>& vecGO)
{
	this->players = vecGO;
}