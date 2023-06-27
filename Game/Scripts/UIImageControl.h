#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentButton;
class ComponentImage;
class ModuleInput;

class UIImageControl : public Script
{
public:
	UIImageControl();
	~UIImageControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	float powerupTimer = 0;
	float time = 0;
	bool powerupStatus = false;
	bool hudStatus = false; // This is to show the complete HUD just hardcode
	bool check; // This is to show the complete HUD just hardcode

	GameObject* disableImgObject;
	GameObject* enableImgObject;
	GameObject* enableImgObject02; //Hardcode for pwr up
	GameObject* enableImgObject03; //Hardcode for pwr up
	GameObject* enableImgObject04; //Hardcode for pwr up
	ComponentImage* imageComponent;
	ModuleInput* input;
};











