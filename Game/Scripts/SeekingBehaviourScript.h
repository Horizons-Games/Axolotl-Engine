#pragma once

#include "Scripting\Script.h"

// This script performs a seeking behaviour once the player enters in range

class SeekingBehaviourScript : public Script
{
public:
	SeekingBehaviourScript();
	~SeekingBehaviourScript() override = default;

	void Init() override {};
	void Start() override {};
	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

private:

};