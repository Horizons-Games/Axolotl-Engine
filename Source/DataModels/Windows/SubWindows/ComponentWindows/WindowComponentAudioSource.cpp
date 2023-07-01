#include "StdAfx.h"

#include "WindowComponentAudioSource.h"
#include "Components/ComponentAudioSource.h"

WindowComponentAudioSource::WindowComponentAudioSource(ComponentAudioSource* component) :
	ComponentWindow("AUDIO SOURCE", component)
{
}

WindowComponentAudioSource::~WindowComponentAudioSource()
{
}

void WindowComponentAudioSource::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentAudioSource* audioSource = static_cast<ComponentAudioSource*>(component);

	if (audioSource)
	{
		ImGui::Text("");
		ImGui::Text("Audio Source");
	}
}
