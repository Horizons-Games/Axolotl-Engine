#include "GameManager.h"

std::unique_ptr<GameManager> GameManager::instance = nullptr;

GameManager::GameManager()
{
}

GameManager* GameManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = std::unique_ptr<GameManager>(new GameManager());
	}
	return instance.get();
}
