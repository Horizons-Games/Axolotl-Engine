#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;

class PlayerMovilityScript : public Script
{

public:
	PlayerMovilityScript();
	~PlayerMovilityScript() override = default;

	void Start() override;
	void PreUpdate(float deltaTime) override;

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