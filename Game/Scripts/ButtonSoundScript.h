#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;
class ComponentButton;
class ConnectedCallback;

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
	void OnClickedCallback();

private:
	ComponentAudioSource* buttonAudio;
	ComponentButton* buttonComponent;

	ButtonStates buttonState;

	std::unique_ptr<ConnectedCallback> connectedCallback;
};