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
	bool hudStatus = false;
	bool check;

	GameObject* DisableImgObject;
	GameObject* EnableImgObject;
	ComponentImage* ImageComponent;
	ModuleInput* input;
};











