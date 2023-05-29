#pragma once

#include "Scripting\Script.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "ModuleInput.h"

class ComponentButton;

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
	bool hudStatus = false; //This is to show the complete HUD just hardcode
	bool check; //This is to show the complete HUD just hardcode

	GameObject* DisableImgObject;
	GameObject* EnableImgObject;
	GameObject* EnableImgObject02; //Hardcode for pwr up
	GameObject* EnableImgObject03; //Hardcode for pwr up
	GameObject* EnableImgObject04; //Hardcode for pwr up
	ComponentImage* ImageComponent;
	ModuleInput* input;
};











