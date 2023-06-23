#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class CantinaMusicScript : public Script
{
public:
	CantinaMusicScript();
	~CantinaMusicScript() override = default;

	void Start() override;

private:
	ComponentAudioSource* cantinaAudio;
};