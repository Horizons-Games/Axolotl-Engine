#pragma once

#include "Scripting\Script.h"

// This script handles the movement/rotation and abilities of the player

class ComponentPlayer;
class ComponentAudioSource;
class ComponentScript;


class PlayerMobilityScript : public Script
{

public:
	PlayerMobilityScript();
	~PlayerMobilityScript() {};

	void Start() override;
	void PreUpdate(float deltaTime) override;

	void Rotate();

private:
	ComponentPlayer* componentPlayer;
	ComponentScript* moveScript;
	ComponentScript* rotateScript;
};