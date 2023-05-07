#pragma once

#include "Scripting\Script.h"

// Little fix until we could check if an audio is being reproduced
enum class PlayerActions
{
	IDLE,
	WALKING
};

class ComponentPlayer;
class ComponentTransform;
class ComponentAudioSource;

class PlayerMoveScript :
    public Script
{
public:
	PlayerMoveScript();
	~PlayerMoveScript() override = default;

	void Start() override;
	void PreUpdate(float deltaTime) override;

private:
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

	ComponentAudioSource* componentAudio;
	PlayerActions playerState;
};