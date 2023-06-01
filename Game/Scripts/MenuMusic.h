#pragma once

#include "Scripting\Script.h"

// This script is just to manage the sounds of a button

class ComponentAudioSource;
class ComponentButton;

enum class ButtonStates
{
	HOVER,
	CLICK,
	OUTSIDE
};

class MenuMusic : public Script
{
public:
	MenuMusic();
	~MenuMusic() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};