#pragma once

#include "Scripting\Script.h"

// This script is just to play the music of the cantina

class ComponentAudioSource;

class TriggerSewersMusic : public Script
{
public:
	TriggerSewersMusic();
	~TriggerSewersMusic() override = default;

	void Start() override;

	bool isMusicTriggered;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;

	ComponentAudioSource* componentAudio;
};