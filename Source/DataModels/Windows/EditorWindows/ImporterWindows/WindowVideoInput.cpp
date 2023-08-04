#include "StdAfx.h"
#include "WindowVideoInput.h"

WindowVideoInput::WindowVideoInput() : WindowFileBrowser()
{
	dialogName = "Select Video";
	title = "Select Video";
	filters = "Video files (*.avi *.mp4){.avi,.mp4}";
	startPath = "Assets/Videos";
}

WindowVideoInput::~WindowVideoInput()
{
}

void WindowVideoInput::DoThisIfOk()
{
	this->isLoading = false;
}
