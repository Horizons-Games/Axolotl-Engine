#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class InputVisualType 
{
	LIGHT,
	HEAVY
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
	void SetComboBarValue(int value);

	void AddInputVisuals(InputVisualType type);

	void ClearCombo(bool finish);

	void CleanInputVisuals();

private:
	GameObject* inputPrefabSoft;
	GameObject* inputPrefabHeavy;

	std::deque<GameObject*> inputVisuals;
	std::vector<GameObject*> inputPositions;

	ComponentSlider* comboBar;

	float clearComboTimer;
	bool clearCombo;
};