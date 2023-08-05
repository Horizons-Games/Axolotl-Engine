#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class AttackType
{
	NONE,
	LIGHTNORMAL,
	HEAVYNORMAL,
	JUMPNORMAL,
	LIGHTFINISHER,
	HEAVYFINISHER,
	JUMPFINISHER
};

class UIComboManager;
class ModuleInput;

class ComboManager : public Script
{
public:
	ComboManager();
	~ComboManager() override = default;

	void Start() override;

	int GetComboCount() const;

	bool NextIsSpecialAttack() const;

	void CheckSpecial(float deltaTime);
	bool IsSpecialActivated() const;
	AttackType CheckAttackInput(bool jumping);
	void SuccessfulAttack(float specialCount, AttackType type);

private:
	void ClearCombo(bool finisher);

	UIComboManager* uiComboManager;

	ModuleInput* input;
	bool specialActivated;
	int specialCount;
	float maxSpecialCount;
	int comboCount;
	float maxComboCount;

	float comboTime;
	float actualComboTimer;
};