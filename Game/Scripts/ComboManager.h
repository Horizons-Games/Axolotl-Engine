#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class AttackType
{
	NONE,
	SOFTNORMAL,
	HEAVYNORMAL,
	JUMPATTACK,
	SOFTFINISHER,
	HEAVYFINISHER
};

class UIComboManager;

class ComboManager : public Script
{
public:
	ComboManager();
	~ComboManager() override = default;

	int GetcomboCount() const;

	void CheckSpecial(float deltaTime);
	AttackType CheckAttackInput(bool jumping);
	void SuccessfulAttack(float specialCount, bool type);

private:
	void Start() override;
	void Update(float deltaTime) override;

	GameObject* comboManagerUIReference;
	UIComboManager* uiComboManager;

	ModuleInput* input;
	bool specialActivated;
	int specialCount;
	float maxSpecialCount;
	int comboCount;
	int maxComboCount;

	float comboTime;
	float actualComboTimer;
};