#include "StdAfx.h"

#include "Components/UI/ComponentVideo.h"
#include "WindowComponentVideo.h"


WindowComponentVideo::WindowComponentVideo(ComponentVideo* component) :
	ComponentWindow("VIDEO", component)
{
}

WindowComponentVideo::~WindowComponentVideo()
{
}

void WindowComponentVideo::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
