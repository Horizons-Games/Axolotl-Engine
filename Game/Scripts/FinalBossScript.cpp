#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossState(FinalBossStates::NEUTRAL)
{
}

void FinalBossScript::Start()
{
}

void FinalBossScript::Update(float deltaTime)
{
	if (bossState == FinalBossStates::NEUTRAL)
	{
		// Patroll
	}
}