#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;

class PlayerMovilityScript : public Script
{

public:
	PlayerMovilityScript();
	~PlayerMovilityScript() override = default;

	void Init() override {};
	void Start() override;
	void PreUpdate(float deltaTime) override;
	void Update(float deltaTime) override {};
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

	void Move();
	void Rotate();

	float GetSpeed() const;
	void SetSpeed(float speed);

private:
	ComponentPlayer* componentPlayer;
	float speed;

};

float PlayerMovilityScript::GetSpeed() const
{
	return speed;
}

void PlayerMovilityScript::SetSpeed(float speed)
{
	this->speed = speed;
}

