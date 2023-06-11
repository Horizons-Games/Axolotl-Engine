#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameManager : public Script
{
public:
	GameManager();
	~GameManager() override = default;

	void Start() override;
	void Update(float deltaTime) override;
};