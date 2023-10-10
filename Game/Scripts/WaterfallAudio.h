#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class WaterfallAudio : public Script
{
public:
	WaterfallAudio();
	~WaterfallAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};