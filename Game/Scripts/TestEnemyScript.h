#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;

class TestEnemyScript : public Script
{

public:
	TestEnemyScript();
	~TestEnemyScript() override = default;

	void Init() override {};
	void PreUpdate(float deltaTime) override;
	void Update(float deltaTime) override {};
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

private:

};


