#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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