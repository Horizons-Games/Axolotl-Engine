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
	if (outerArea->GetAreaState() == AreaState::ON_COOLDOWN || 
		innerArea->GetAreaState() == AreaState::ON_COOLDOWN)
	{
		shockWaveCooldown -= deltaTime;

		if (shockWaveCooldown <= 0.0f)
		{
			outerArea->SetAreaState(AreaState::IDLE);
			innerArea->SetAreaState(AreaState::IDLE);
			shockWaveCooldown = shockWaveMaxCooldown;
		}
	}
}

void ShockWaveAttackScript::TriggerShockWaveAttack()
{
	outerArea->SetAreaState(AreaState::EXPANDING);
	innerArea->SetAreaState(AreaState::EXPANDING);
}

bool ShockWaveAttackScript::CanPerformShockWaveAttack() const
{
	return outerArea->GetAreaState() == AreaState::IDLE &&
			innerArea->GetAreaState() == AreaState::IDLE;
}