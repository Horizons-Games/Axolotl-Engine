#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"

RUNTIME_MODIFIABLE_INCLUDE;

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

	std::string tag;
	std::vector<GameObject*> enemiesGameObjects;
};