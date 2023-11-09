#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class WaterBubblesAudio : public Script
{
public:
	WaterBubblesAudio();
	~WaterBubblesAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};