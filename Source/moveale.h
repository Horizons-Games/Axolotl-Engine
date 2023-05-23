#pragma once

#include "Scripting\Script.h"

// This script handles the movement/rotation and abilities of the player

class ComponentPlayer;
class ComponentAudioSource;
class ComponentAnimation;

// Little fix until we could check if an audio is being reproduced
enum class PlayerActions
{
	IDLE,
	WALKING
};

class MoveAle : public Script
{
public:
	MoveAle();
	~MoveAle();

	void Start() override;
	void Update(float deltaTime) override;

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

	bool isDashing;

	PlayerActions playerState;
};