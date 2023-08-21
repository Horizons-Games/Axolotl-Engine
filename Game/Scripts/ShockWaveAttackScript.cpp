#include "StdAfx.h"
#include "ShockWaveAttackScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/ShockWaveAttackAreaScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(ShockWaveAttackScript);

ShockWaveAttackScript::ShockWaveAttackScript() : Script(), outerArea(nullptr), innerArea(nullptr),
	shockWaveCooldown(0.0f), shockWaveMaxCooldown(5.0f), shockWaveHitPlayer(false), shockWaveDamage(10.0f)
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
	// To deal damage to the player, both areas should be expanding
	// But only the outside area should be detecting the player
	if (outerArea->GetAreaState() == AreaState::EXPANDING && innerArea->GetAreaState() == AreaState::EXPANDING &&
		outerArea->GetPlayerDetected() && !innerArea->GetPlayerDetected() && !shockWaveHitPlayer)
	{
		GameObject* playerHit = outerArea->GetPlayerDetected();
		playerHit->GetComponent<HealthSystem>()->TakeDamage(shockWaveDamage);

		shockWaveHitPlayer = true;
	}

	else if (outerArea->GetAreaState() == AreaState::ON_COOLDOWN && 
		innerArea->GetAreaState() == AreaState::ON_COOLDOWN)
	{
		shockWaveCooldown -= deltaTime;

		if (shockWaveCooldown <= 0.0f)
		{
			outerArea->SetAreaState(AreaState::IDLE);
			innerArea->SetAreaState(AreaState::IDLE);
			shockWaveCooldown = shockWaveMaxCooldown;
			shockWaveHitPlayer = false;
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