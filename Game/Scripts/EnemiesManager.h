#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class Scene;

class EnemiesManager : public Script
{
public:
	EnemiesManager();
	~EnemiesManager() override = default;

	void PauseEnemies(bool paused);

	const std::vector<GameObject*>& GetVectorEnemiesGameObjects() const;
	void SetVectorEnemiesGameObjects(const std::vector<GameObject*>& enemiesGameObjects);

private:
	void Start() override;

	Scene* scene;

	std::string enemyTag;
	std::string bulletTag;
	std::string alluraBulletTag;
	std::vector<GameObject*> enemiesGameObjects;
	std::vector<GameObject*> bulletGameObjects;
	std::vector<GameObject*> alluraBulletGameObjects;
};