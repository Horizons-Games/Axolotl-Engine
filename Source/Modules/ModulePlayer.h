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
	void SetPlayer(GameObject* player);
	Camera* GetCameraPlayer();

	void LoadNewPlayer();
	void UnloadNewPlayer();

	bool IsStatic();
	void CheckIfActivateMouse();

private:
	GameObject* player;
	Camera* cameraPlayer;
	ComponentPlayer* componentPlayer;
};
