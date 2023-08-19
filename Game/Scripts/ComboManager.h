#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

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

class ComboManager : public Script
{
public:
	ComboManager();
	~ComboManager() override = default;

	void Init() override;
	void Start() override;

	int GetComboCount() const;

	bool NextIsSpecialAttack() const;

	void CheckSpecial(float deltaTime);
	AttackType CheckAttackInput(bool jumping);
	void SuccessfulAttack(float specialCount, AttackType type);
	bool IsSpecialActivated() const;

private:
	void ClearCombo(bool finisher);

	UIComboManager* uiComboManager;
	KeyState keyState;

	ModuleInput* input;
	bool specialActivated;
	float specialCount;
	float maxSpecialCount;
	int comboCount;
	float maxComboCount;

	float comboTime;
	float actualComboTimer;
};