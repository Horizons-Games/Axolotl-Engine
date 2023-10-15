#include "StdAfx.h"

#include "Components/UI/ComponentVideo.h"
#include "WindowComponentVideo.h"
#include "Windows/EditorWindows/ImporterWindows/WindowVideoInput.h"


WindowComponentVideo::WindowComponentVideo(ComponentVideo* component) :
	ComponentWindow("VIDEO", component),
	inputVideo(std::make_unique<WindowVideoInput>(component)),
	loop(component->IsLooping()),
	verticalRotate(component->CanRotateVertical()),
	playAtStart(false)
{
}

WindowComponentVideo::~WindowComponentVideo()
{
}

void WindowComponentVideo::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentVideo* videoComponent = static_cast<ComponentVideo*>(component);
	if (!videoComponent)
	{
		return;
	}

	bool playAtStart = videoComponent->GetPlayAtStart();
	if (videoComponent->GetVideo() != nullptr)
	{
		if (ImGui::Checkbox("Play at start", &playAtStart))
		{
			videoComponent->SetPlayAtStart(playAtStart);
		}
		if (ImGui::ArrowButton("##Play", ImGuiDir_Right))
		{
			if (videoComponent->isPlayed())
			{
				videoComponent->Stop();
			}
			else
			{
				videoComponent->Play();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("||"))
		{
			videoComponent->Pause();
		}
		if (ImGui::Button("X"))
		{
			videoComponent->SetVideo(nullptr);
			return;
		}
		ImGui::SameLine();
		ImGui::Text("Path Asset:");
		ImGui::SameLine();
		ImGui::Text(videoComponent->GetVideo()->GetAssetsPath().c_str());

		if (ImGui::Button("Native Size"))
		{
			videoComponent->SetNativeVideoFrameSize();
		}
	}
	else
	{
		inputVideo->DrawWindowContents();
	}

	if (ImGui::Checkbox("Loop", &loop))
	{
		videoComponent->SetLoop(loop);
	}
	if (videoComponent->CanBeRotate())
	{
		if (ImGui::Checkbox("Rotate Vertical", &verticalRotate))
		{
			videoComponent->SetRotateVertical(verticalRotate);
		}
	}
}
