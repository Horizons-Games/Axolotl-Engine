#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class TurbineSpaceshipEscapeAudio : public Script
{
public:
	TurbineSpaceshipEscapeAudio();
	~TurbineSpaceshipEscapeAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};