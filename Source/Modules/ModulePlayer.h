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
	GameObject* GetCameraPlayerObject();

	bool LoadNewPlayer();
	void UnloadNewPlayer();

	bool IsStatic();
	void CheckIfActivateMouse();

	void SetInCombat(bool newCombat);
	void SetInBossCombat(bool newBoss);
	bool IsInCombat();
	bool IsInBossCombat();

	void SetEnemiesToDefeat(float newEnemiesToDefeat);
	int GetEnemiesToDefeat();

private:
	GameObject* player;
	Camera* cameraPlayer;
	GameObject* cameraPlayerObject;
	ComponentPlayer* componentPlayer;

	//Combat
	bool inCombat;
	bool inBossCombat;
	int enemiesToDefeat;
};

int inline ModulePlayer::GetEnemiesToDefeat()
{
	return enemiesToDefeat;
}