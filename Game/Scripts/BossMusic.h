#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class BossMusic : public Script
{
public:
	BossMusic();
	~BossMusic() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};