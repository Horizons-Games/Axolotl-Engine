#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;

class TriggerHangarMusic : public Script
{
public:
	TriggerHangarMusic();
	~TriggerHangarMusic() override = default;

	void Start() override;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
};