#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;
class ComponentButton;

enum class ButtonStates
{
	HOVER,
	CLICK,
	OUTSIDE
};

class ButtonSoundScript : public Script
{
public:
	ButtonSoundScript();
	~ButtonSoundScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentAudioSource* buttonAudio;
	ComponentButton* buttonComponent;

	ButtonStates buttonState;
};