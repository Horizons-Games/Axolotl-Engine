#include "StdAfx.h"

#include "Components/UI/ComponentVideo.h"
#include "WindowComponentVideo.h"
#include "Windows/EditorWindows/ImporterWindows/WindowVideoInput.h"


WindowComponentVideo::WindowComponentVideo(ComponentVideo* component) :
	ComponentWindow("VIDEO", component),
	inputVideo(std::make_unique<WindowVideoInput>())
	//inputVideo(std::make_unique<WindowVideoInput>(component))
{
}

WindowComponentVideo::~WindowComponentVideo()
{
}

void WindowComponentVideo::DrawWindowContents()
{
	ComponentVideo* videoComponent = static_cast<ComponentVideo*>(component);
	//DrawEnableAndDeleteComponent();
	if (videoComponent->GetVideo() != nullptr)
	{
	}
	else
	{
		inputVideo->DrawWindowContents();
	}
}
