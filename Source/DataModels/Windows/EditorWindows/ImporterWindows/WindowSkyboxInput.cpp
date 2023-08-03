#include "StdAfx.h"

#include "WindowSkyboxInput.h"
#include "Defines/ExtensionDefines.h"


WindowSkyboxInput::WindowSkyboxInput() : WindowFileBrowser()
{
	dialogName = "Change Skybox";
	title = "Change Skybox";
	filters = SKYBOX_EXTENSION;
	startPath = "Assets/Skybox";
}
WindowSkyboxInput::~WindowSkyboxInput()
{
}
void WindowSkyboxInput::DoThisIfOk()
{
}