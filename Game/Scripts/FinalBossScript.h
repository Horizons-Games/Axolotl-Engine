#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class FinalBossStates
{
	NEUTRAL,
	AGGRESIVE,
	DEFENSIVE,
	LAST_RESOLT
};

class FinalBossScript : public EnemyClass
{
public:
	FinalBossScript();
	~FinalBossScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	FinalBossStates bossState;
};