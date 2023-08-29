#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;
class ComponentRigidBody;


class CombatZoneScript : public Script
{
public:
	CombatZoneScript();
	~CombatZoneScript();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;

private:
	ComponentAudioSource* componentAudio;
	ComponentRigidBody* componentRigidBody;
	float enemiesToDefeat;
};