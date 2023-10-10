#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class TurbineSewersAudio : public Script
{
public:
	TurbineSewersAudio();
	~TurbineSewersAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};