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

	float GetDashForce() const;
	void SetDashForce(float dashForce);

	bool GetCanDash() const;
	void SetCanDash(bool canDash);

	bool GetCanDoubleJump() const;
	void SetCanDoubleJump(bool canDoubleJump);

private:
	ComponentPlayer* componentPlayer;
	float speed;
	float jumpParameter;
	float dashForce;
	float nextDash;
	bool canDash;
	bool canDoubleJump;
	bool isCrouch;
	unsigned int jumps;

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

inline float PlayerMovilityScript::GetDashForce() const
{
	return dashForce;
}

inline void PlayerMovilityScript::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

inline bool PlayerMovilityScript::GetCanDash() const
{
	return canDash;
}

inline void PlayerMovilityScript::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

inline bool PlayerMovilityScript::GetCanDoubleJump() const
{
	return canDoubleJump;
}

inline void PlayerMovilityScript::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}


