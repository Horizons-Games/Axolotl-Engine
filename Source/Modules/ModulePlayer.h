#pragma once
#include "Module.h"

class GameObject;
class Camera;
class ComponentPlayer;

class ModulePlayer : public Module
{
public:
	ModulePlayer();
	~ModulePlayer() override;

	bool Start() override;

	GameObject* GetPlayer();
	GameObject* GetSecondPlayer();
	void SetPlayer(GameObject* player);
	void SetSecondPlayer(GameObject* player);
	Camera* GetCameraPlayer();
	GameObject* GetCameraPlayerObject();

	bool LoadNewPlayer();
	void UnloadNewPlayer();

	bool IsStatic();
	void CheckIfActivateMouse();

private:
	GameObject* player0;
	GameObject* player1;
	Camera* cameraPlayer;
	GameObject* cameraPlayerObject;
	ComponentPlayer* componentPlayer;
};
