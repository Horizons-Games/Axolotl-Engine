#include "StdAfx.h"
#include "WindowFontInput.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceFont.h"

WindowFontInput::WindowFontInput() :
	WindowFileBrowser()
{
	dialogName = "Select Font";
	title = "Load Font";
	filters = FONT_EXTENSION + std::string(",.TTF,.*");
	startPath = "Assets/Fonts";	
}

WindowFontInput::~WindowFontInput()
{
}

void WindowFontInput::DoThisIfOk()
{	
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceFont> font =
			std::dynamic_pointer_cast<ResourceFont>(App->GetModule<ModuleResources>()->ImportResource(filePath));		
	
	this->isLoading = false;
}