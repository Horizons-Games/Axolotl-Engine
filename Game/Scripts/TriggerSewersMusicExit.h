#pragma once

#include "Scripting\Script.h"

// This script is just to play the music of the cantina

class ComponentAudioSource;
class TriggerSewersMusic;
class GameObject;

class TriggerSewersMusicExit : public Script
{
public:
	TriggerSewersMusicExit();
	~TriggerSewersMusicExit() override = default;

	void Start() override;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;

	ComponentAudioSource* componentAudio;

	TriggerSewersMusic* triggerSewersMusicScript;
	GameObject* triggerSewersMusicGO;
};