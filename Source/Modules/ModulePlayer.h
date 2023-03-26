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

	bool Init() override;
	bool Start() override;

	update_status PreUpdate();

	void Move();
	void Rotate();

	GameObject* GetPlayer();
	void SetPlayer(std::unique_ptr<GameObject> player);
	Camera* GetCameraPlayer();


private:
	std::unique_ptr <GameObject>  player;
	Camera* cameraPlayer;
	ComponentPlayer* componentPlayer;
	float speed = 3;
	
};
