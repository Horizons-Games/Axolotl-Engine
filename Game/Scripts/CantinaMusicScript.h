#pragma once

#include "Scripting\Script.h"

// This script is just to play the music of the cantina

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