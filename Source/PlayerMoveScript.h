#pragma once
#include "Script.h"

class ComponentPlayer;
class ComponentTransform;

class PlayerMoveScript :
    public Script
{
	PlayerMoveScript();
	~PlayerMoveScript();

	void Start() override;
	void PreUpdate(float deltaTime) override;

	void Move();

private:
	ComponentTransform* componentTransform;

	float speed;
	float jumpParameter;
	float dashForce;
	float nextDash;
	bool canDash;
	bool canDoubleJump;
	bool isCrouch;
	unsigned int jumps;

};

