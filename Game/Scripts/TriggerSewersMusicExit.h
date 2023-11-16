#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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

	TriggerSewersMusic* triggerSewersMusicScript;
	GameObject* triggerSewersMusicGO;
};