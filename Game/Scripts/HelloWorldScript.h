#pragma once
#include "Scripting\Script.h"
#include "RuntimeInclude.h"
RUNTIME_MODIFIABLE_INCLUDE;

// This script performs what it was recommended that a basic script should do in the Gameplay Systems PPT

class HelloWorldScript : public Script
{
public:
	HelloWorldScript();
	~HelloWorldScript() override = default;

	void Update(float deltaTime) override;

private:
	std::string helloWorld;
	GameObject* movingGameObject;
};