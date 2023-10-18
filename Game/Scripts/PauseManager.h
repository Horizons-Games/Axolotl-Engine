#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class Scene;

class PauseManager : public Script
{
public:
	PauseManager();
	~PauseManager() override = default;

	void Pause(bool paused);
	void PauseBullets(bool paused);
	void PauseRocks(bool paused);
	void PauseMissiles(bool paused);
	void PauseEnemies(bool paused);

private:
	void Start() override;

	Scene* scene;

	std::string enemyTag;
	std::string bulletTag;
	std::string alluraBulletTag;
	std::string rockTag;
	std::string missileTag;
	std::vector<GameObject*> enemiesGameObjects;
	std::vector<GameObject*> bulletGameObjects;
	std::vector<GameObject*> alluraBulletGameObjects;
	std::vector<GameObject*> rockGameObjects;
	std::vector<GameObject*> missileGameObjects;
};