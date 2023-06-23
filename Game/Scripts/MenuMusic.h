#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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