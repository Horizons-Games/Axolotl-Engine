#include "WindowComponentAudioListener.h"
#include "Components/ComponentAudioListener.h"

WindowComponentAudioListener::WindowComponentAudioListener(ComponentAudioListener* component) :
	ComponentWindow("Audio Source", component)
{
}

WindowComponentAudioListener::~WindowComponentAudioListener()
{
}

void WindowComponentAudioListener::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentAudioListener* audioSource = static_cast<ComponentAudioListener*>(component);

	if (audioSource)
	{
		ImGui::Text("");
		ImGui::Text("Audio Listener");
	}
}

