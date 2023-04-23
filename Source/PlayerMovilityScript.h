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

	float GetJumpParameter() const;
	void SetJumpParameter(float jumpParameter);

private:
	ComponentPlayer* componentPlayer;
	float speed;
	float jumpParameter;

};

float PlayerMovilityScript::GetSpeed() const
{
	return speed;
}

void PlayerMovilityScript::SetSpeed(float speed)
{
	this->speed = speed;
}

float PlayerMovilityScript::GetJumpParameter() const
{
	return jumpParameter;
}

void PlayerMovilityScript::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}


