#include "ButtonSoundScript.h"

#include "Components/ComponentAudioSource.h"
#include "Components/UI/ComponentButton.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(ButtonSoundScript);

ButtonSoundScript::ButtonSoundScript() : Script(), buttonAudio(nullptr), buttonComponent(nullptr),
buttonState(ButtonStates::OUTSIDE)
{

}

void ButtonSoundScript::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	buttonAudio = owner->GetComponent<ComponentAudioSource>();
}

void ButtonSoundScript::Update(float deltaTime)
{
	if (buttonComponent->IsHovered() && buttonState == ButtonStates::OUTSIDE)
	{
		buttonAudio->PostEvent(audio::SFX_UI_BUTTON_HOVER);
		buttonState = ButtonStates::HOVER;
	}

	if (buttonComponent->IsClicked() && buttonState == ButtonStates::HOVER)
	{
		buttonAudio->PostEvent(audio::SFX_UI_BUTTON_PRESS);
		buttonState = ButtonStates::CLICK;
	}

	if (!buttonComponent->IsClicked())
	{
		buttonState = ButtonStates::HOVER;
	}

	if (!buttonComponent->IsHovered())
	{
		buttonState = ButtonStates::OUTSIDE;
	}
}