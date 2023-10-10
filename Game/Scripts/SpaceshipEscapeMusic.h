#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class SpaceshipEscapeMusic : public Script
{
public:
	SpaceshipEscapeMusic();
	~SpaceshipEscapeMusic() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};