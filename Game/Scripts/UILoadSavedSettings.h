#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class UILoadSavedSettings : public Script
{
public:
	UILoadSavedSettings();
	~UILoadSavedSettings() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:

	GameObject* optionsMenu;

};