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
class ModuleInput;

class ComboManager : public Script
{
public:
	ComboManager();
	~ComboManager() override = default;

	int GetcomboCount() const;

	void CheckSpecial(float deltaTime);
	bool isSpecialActivated() const;
	AttackType CheckAttackInput(bool jumping);
	void SuccessfulAttack(float specialCount, AttackType type);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void ClearCombo(bool finisher);

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