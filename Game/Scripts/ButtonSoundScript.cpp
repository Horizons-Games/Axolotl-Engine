#include "StdAfx.h"

#include "ButtonSoundScript.h"

#include "Components/ComponentAudioSource.h"
#include "Components/UI/ComponentButton.h"

#include "Auxiliar/Audio/AudioData.h"
#include "Auxiliar/ConnectedCallback.h"

REGISTERCLASS(ButtonSoundScript);

ButtonSoundScript::ButtonSoundScript() : Script(), buttonAudio(nullptr), buttonComponent(nullptr),
buttonState(ButtonStates::OUTSIDE)
{

}

void ButtonSoundScript::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	buttonAudio = owner->GetComponent<ComponentAudioSource>();

	connectedCallback = buttonComponent->SetOnClickedCallback(std::bind(&ButtonSoundScript::OnClickedCallback, this));
}

void ButtonSoundScript::Update(float deltaTime)
{
	if (buttonComponent->IsHovered() && buttonState == ButtonStates::OUTSIDE)
	{
		buttonAudio->PostEvent(AUDIO::SFX::UI::BUTTON_HOVER);
		buttonState = ButtonStates::HOVER;
	}
	else if (!buttonComponent->IsHovered())
	{
		buttonState = ButtonStates::OUTSIDE;
	}
}

void ButtonSoundScript::OnClickedCallback()
{
	if (buttonComponent->IsClicked() && buttonState == ButtonStates::HOVER)
	{
		buttonAudio->PostEvent(AUDIO::SFX::UI::BUTTON_PRESS);
		buttonState = ButtonStates::CLICK;
	}

	if (!buttonComponent->IsClicked())
	{
		buttonState = ButtonStates::HOVER;
	}
}