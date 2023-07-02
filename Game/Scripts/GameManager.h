#pragma once

#include <memory>

class GameManager
{
public:
	~GameManager() = default;

	static GameManager* GetInstance();

private:
	GameManager();

	static std::unique_ptr<GameManager> instance;
};
