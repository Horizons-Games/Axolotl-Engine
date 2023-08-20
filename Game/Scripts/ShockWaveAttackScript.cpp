#include "StdAfx.h"
#include "ShockWaveAttackScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/ShockWaveAttackAreaScript.h"

REGISTERCLASS(ShockWaveAttackScript);

ShockWaveAttackScript::ShockWaveAttackScript() : Script(), outerArea(nullptr), innerArea(nullptr),
	shockWaveCooldown(0.0f), shockWaveMaxCooldown(5.0f)
{
	REGISTER_FIELD(shockWaveMaxCooldown, float);

	REGISTER_FIELD(outerArea, ShockWaveAttackAreaScript*);
	REGISTER_FIELD(innerArea, ShockWaveAttackAreaScript*);
}

void ShockWaveAttackScript::Start()
{
	shockWaveCooldown = shockWaveMaxCooldown;
}

void ShockWaveAttackScript::Update(float deltaTime)
{
	if ((outerArea->GetMaxExpansionReached() || innerArea->GetMaxExpansionReached()) && 
		shockWaveCooldown >= 0.0f)
	{
		shockWaveCooldown -= deltaTime;
	}

	else
	{
		shockWaveCooldown = shockWaveMaxCooldown;
	}
}

void ShockWaveAttackScript::TriggerShockWaveAttack() const
{
	outerArea->TriggerAreaExpansion();
	innerArea->TriggerAreaExpansion();
}

bool ShockWaveAttackScript::CanPerformShockWaveAttack() const
{
	return shockWaveCooldown <= 0.0f;
}