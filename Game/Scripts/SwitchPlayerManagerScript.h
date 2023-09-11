#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Timer/Timer.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class CameraControllerScript;
class ComponentTransform;
class ComponentRigidBody;
class PlayerMoveScript;
class PlayerJumpScript;
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

	void ChangeCurrentPlayer();
	void IsChangingCurrentPlayer();

private:
	void Start() override;

	int currentPlayerID;

	ModuleInput* input;
	Timer changePlayerTimer;

	GameObject* mainCamera;
	CameraControllerScript* camera;
	ComponentTransform* cameraTransform;

	std::vector<GameObject*> players;
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;

	btVector3 rigidBodyVec3;

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
