#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class InputVisualType 
{
	LIGHT,
	HEAVY,
	JUMP
};

class GameObject;
class ComponentSlider;

class UIComboManager : public Script
{
public:
	UIComboManager();
	~UIComboManager() override = default;

	void Init() override;
	void Update(float deltaTime) override;

	float GetMaxComboBarValue() const;

	void SetActivateSpecial(bool activate);
	void SetComboBarValue(float value);

	void AddInputVisuals(InputVisualType type);

	void ClearCombo(bool finish);

	void CleanInputVisuals();

	void UpdateFadeOut(float transparency);

	void FinishComboButtonsEffect();
	void SetEffectEnable(bool effectEnabled);
	bool IsCombatActive();


private:
	GameObject* inputPrefabSoft;
	GameObject* inputPrefabHeavy;
	GameObject*  inputPrefabJumpAttack;

	GameObject* noFillBar;
	std::vector<GameObject*> shinyButtonEffect;

	std::deque<GameObject*> inputVisuals;
	std::vector<GameObject*> inputPositions;

	ComponentSlider* comboBar;

	float clearComboTimer;
	bool clearCombo;
	bool  isEffectEnabled;
	bool forceEnableComboBar;

	float transparency;
	bool alphaEnabled;
};

inline void UIComboManager::SetEffectEnable(bool effectEnabled)
{
	isEffectEnabled = effectEnabled;
};