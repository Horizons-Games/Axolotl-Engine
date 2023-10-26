#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class HealthSystem;
class UIImageDisplacementControl;
class ComponentSlider;

enum HPMode
{
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4
};

class UIEnemyBar : public Script
{

public:
	UIEnemyBar();
	~UIEnemyBar();

	void Start() override;
	void Update(float deltaTime) override;

	void UpdateBar();
	void CheckMode();
	void ChangeMode();
	void SetAppearNextCombat(bool nextCombat);

private:

	HealthSystem* boss;
	float firstChange;
	float secondChange;
	float thirdChange;
	ComponentSlider* slider;
	HPMode actualMode;
	bool alwaysActive;
	int barValue;
	GameObject* icons;
	bool lastTickInCombat;
	bool appearNextCombat;
	UIImageDisplacementControl* displacement;

};