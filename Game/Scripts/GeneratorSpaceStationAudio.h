#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class GeneratorSpaceStationAudio : public Script
{
public:
	GeneratorSpaceStationAudio();
	~GeneratorSpaceStationAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};