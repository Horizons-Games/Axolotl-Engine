#pragma once

#include "Scripting\Script.h"

class ComponentAudioSource;
class ModuleInput;

class BixAttackScript : public Script
{

public:
	BixAttackScript();
	~BixAttackScript() override = default;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();


	ComponentAudioSource* componentAudio;

	ModuleInput* input;

};