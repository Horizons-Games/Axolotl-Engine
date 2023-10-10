#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class SpaceStationMusic : public Script
{
public:
	SpaceStationMusic();
	~SpaceStationMusic() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};