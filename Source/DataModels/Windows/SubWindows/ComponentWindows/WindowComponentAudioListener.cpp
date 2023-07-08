#include "StdAfx.h"

#include "WindowComponentAudioListener.h"
#include "Components/ComponentAudioListener.h"

WindowComponentAudioListener::WindowComponentAudioListener(ComponentAudioListener* component) :
	ComponentWindow("AUDIO LISTENER", component)
{
}

WindowComponentAudioListener::~WindowComponentAudioListener()
{
}

void WindowComponentAudioListener::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentAudioListener* audioListener = static_cast<ComponentAudioListener*>(component);

	if (audioListener)
	{
		ImGui::Text("");
		ImGui::Text("Audio Listener");
	}
}
