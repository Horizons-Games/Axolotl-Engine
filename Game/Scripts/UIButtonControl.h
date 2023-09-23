#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentButton;
class ComponentScript;
class UIGameManager;
class SceneLoadingScript;

class UIButtonControl : public Script
{
public:
	UIButtonControl();
	~UIButtonControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentButton* buttonComponent;
	UIGameManager* UIGameManagerClass;

	bool isGameExit;
	bool isGameResume;

	GameObject* disableObject;
	GameObject* enableObject;
	GameObject* buttonHover;
	GameObject* setUiGameManagerObject;
	SceneLoadingScript* loadingScreenScript;
};











