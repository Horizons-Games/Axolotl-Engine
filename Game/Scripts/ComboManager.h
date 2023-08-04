#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class AttackType
{
	NONE,
	SOFTNORMAL,
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

	float GetcomboCount() const;

	bool NextIsSpecialAttack() const;

	void CheckSpecial(float deltaTime);
	bool IsSpecialActivated() const;
	AttackType CheckAttackInput(bool jumping);
	void SuccessfulAttack(float specialCount, AttackType type);

private:
	void ClearCombo(bool finisher);

	GameObject* comboManagerUIReference;
	UIComboManager* uiComboManager;

	ModuleInput* input;
	bool specialActivated;
	float specialCount;
	float maxSpecialCount;
	float comboCount;
	float maxComboCount;

	float comboTime;
	float actualComboTimer;
};