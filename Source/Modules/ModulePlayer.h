#pragma once
#include "Module.h"

#pragma message(__FILE__ " TODO: Delete this module")

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
	GameObject* GetCameraPlayerObject();

	bool LoadNewPlayer();
	void UnloadNewPlayer();

	bool IsStatic();
	void CheckIfActivateMouse();

private:
	GameObject* player;
	Camera* cameraPlayer;
	GameObject* cameraPlayerObject;
	ComponentPlayer* componentPlayer;
};
