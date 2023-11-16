#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class FireObstaclePermanentAudio : public Script
{
public:
	FireObstaclePermanentAudio();
	~FireObstaclePermanentAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};