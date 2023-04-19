#pragma once

#include "Scripting\Script.h"

// This script performs what it was recommended that a basic script should do in the Gameplay Systems PPT

class HelloWorldScript : public Script
{
public:
	HelloWorldScript();
	~HelloWorldScript() override = default;

	void Init() override {};
	void Start() override {};
	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

	std::string GetHelloWorld() const;
	void SetHelloWorld(const std::string helloWorld);

	GameObject* GetMovingGameObject() const;
	void SetMovingGameObject(GameObject* movingGameObject);

private:
	std::string helloWorld;
	GameObject* movingGameObject;
};