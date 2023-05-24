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
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
	buttonAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
}

void ButtonSoundScript::Update(float deltaTime)
{
	if (buttonComponent->IsHovered() && buttonState == ButtonStates::OUTSIDE)
	{
		buttonAudio->PostEvent(AUDIO::SFX::UI::BUTTON_HOVER);
		buttonState = ButtonStates::HOVER;
	}

	if (buttonComponent->IsClicked() && buttonState == ButtonStates::HOVER)
	{
		buttonAudio->PostEvent(AUDIO::SFX::UI::BUTTON_PRESS);
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