#include "StdAfx.h"

#include "Components/UI/ComponentVideo.h"
#include "WindowComponentVideo.h"
#include "Windows/EditorWindows/ImporterWindows/WindowVideoInput.h"


WindowComponentVideo::WindowComponentVideo(ComponentVideo* component) :
	ComponentWindow("VIDEO", component),
	inputVideo(std::make_unique<WindowVideoInput>(component)),
	loop(component->GetLoop()),
	verticalRotate(component->GetRotateVertical()),
	component(component)
{
}

WindowComponentVideo::~WindowComponentVideo()
{
}

void WindowComponentVideo::DrawWindowContents()
{
	ComponentVideo* videoComponent = static_cast<ComponentVideo*>(component);
	if (videoComponent->GetVideo() != nullptr)
	{
		ImGui::Text("Path Asset:");
		ImGui::SameLine();
		ImGui::Text(videoComponent->GetVideo()->GetAssetsPath().c_str());
	}
	else
	{
		inputVideo->DrawWindowContents();
	}

	if (ImGui::Checkbox("Loop", &loop))
	{
		videoComponent->SetLoop(loop);
	}
	if (component->GetCanBeRotate())
	{
		if (ImGui::Checkbox("Rotate Vertical", &verticalRotate))
		{
			videoComponent->SetRotateVertical(verticalRotate);
		}
	}
}
