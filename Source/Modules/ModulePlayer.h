#pragma once
#include "Module.h"
#include "Components/ComponentPlayer.h"

class GameObject;
class Camera;

class ModulePlayer : public Module
{
public:
	ModulePlayer();
	~ModulePlayer() override;

	bool Init() override;
	bool Start() override;

	update_status PreUpdate();
	update_status Update();

	void Move();
	void Rotate();

	GameObject* GetPlayer();
	void SetPlayer(std::unique_ptr<GameObject> player);
	Camera* GetCameraPlayer();

	void LoadNewPlayer();

	bool IsStatic();

private:
	std::unique_ptr <GameObject>  player;
	Camera* cameraPlayer;
	ComponentPlayer* componentPlayer;
	float speed = 3;
	
};

inline bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}
