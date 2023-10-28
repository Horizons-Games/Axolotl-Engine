#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class HackingMachineAudio : public Script
{
public:
	HackingMachineAudio();
	~HackingMachineAudio() override;

	void Start() override;

private:
	ComponentAudioSource* componentAudio;
};