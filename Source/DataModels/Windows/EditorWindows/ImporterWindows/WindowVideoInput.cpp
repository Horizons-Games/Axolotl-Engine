#include "StdAfx.h"

#include "WindowVideoInput.h"

#include "Components/UI/ComponentVideo.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceVideo.h"

WindowVideoInput::WindowVideoInput(ComponentVideo* video) : 
	WindowFileBrowser(), 
	component(video)
{
	dialogName = "Select Video";
	title = "Select Video";
	filters = "Video files (*.avi *.mp4 *.mov){.avi,.mp4,.mov}";
	startPath = "Assets/Videos";
}

WindowVideoInput::~WindowVideoInput()
{
}

void WindowVideoInput::DoThisIfOk()
{
	this->isLoading = false;
	std::string filePath = std::string(fileDialogImporter.GetFilePathName());
	std::shared_ptr<ResourceVideo> video =
		App->GetModule<ModuleResources>()->RequestResource<ResourceVideo>(filePath);
	component->SetVideo(video);
}
