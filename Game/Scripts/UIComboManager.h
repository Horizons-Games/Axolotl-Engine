#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

#include "ComboManager.h"

RUNTIME_MODIFIABLE_INCLUDE;

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

	void AddInputVisuals(AttackType type);

	void ClearCombo(bool finish);

	void CleanInputVisuals();

	void UpdateFadeOut(float transparency);

	void InitFinishComboButtonsEffect();

	void InitComboUI();
	void HideComboUI();

	std::deque<GameObject*> inputVisuals;
private:
	GameObject* inputPrefabSoft;
	GameObject* inputPrefabHeavy;
	GameObject* inputPrefabJumpAttack;
	GameObject* inputPrefabLightFinisherAttack;
	GameObject* inputPrefabHeavyFinisherAttack;
	GameObject* shinyEffectPrefab;
	GameObject* shinyEffectBarPrefab;

	GameObject* noFillBar;

	std::vector<GameObject*> shinyButtonEffect;
	std::vector<GameObject*> shinyBarEffect;
	std::vector<GameObject*> inputPositions;

	ComponentSlider* comboBar;

	float clearComboTimer;
	bool clearCombo;
	bool finisherClearEffect;

	float transparency;
	bool alphaEnabled;

	bool specialActivated;
};