#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class EnemyClass : public Script
{
public:
	EnemyClass();
	~EnemyClass() override = default;

	void Start() override;
	void Update(float deltaTime) override;
};