#include "WindowFileBrowser.h"
#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "Timer/Timer.h"

void WindowFileBrowser::DrawWindowContents()
{
	//WindowImporter
	if (ImGui::Button(title))
	{
		Uint32 flags = ImGuiFileDialogFlags_Modal;
		if (isSave)
		{
			flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
		}
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", dialogName, filters, startPath,
			"", 1, nullptr, flags);
	}
	// display
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{

		// action if OK
		if (fileDialogImporter.IsOk())
		{
			isLoading = true;
			DoThisIfOk();
		}

		// close
		fileDialogImporter.Close();
	}
	//End of Window importer
	
	if(title == ICON_IGFD_FOLDER " Import Asset")
	{
		Browser();
	}

	ImportResourceWithLoadingWindow();
}

void WindowFileBrowser::Browser()
{
	fileDialogBrowser.OpenDialog("embedded", "Select File", ".*",browserPath, "", -1, nullptr,
		ImGuiFileDialogFlags_NoDialog |
		ImGuiFileDialogFlags_DisableBookmarkMode |
		ImGuiFileDialogFlags_DisableCreateDirectoryButton);
		//ImGuiFileDialogFlags_ReadOnlyFileNameField
	fileDialogBrowser.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350));
		
	if (std::string::npos == fileDialogBrowser.GetCurrentPath().find("Assets"))
	{
		fileDialogBrowser.Close();
	}
	
}

void WindowFileBrowser::DoThisIfOk()
{
	filePathName = fileDialogImporter.GetFilePathName();
	this->ImportResourceAsync(filePathName);
}

void WindowFileBrowser::ImportResourceWithLoadingWindow()
{
	winLoading->Draw(isLoading);
	if (futureResourceUID._Is_ready() && timer->Read() > 1000)
	{
		isLoading = false;
		timer->Stop();
		this->ImportResourceAsync(filePathName);
	}
}

UID WindowFileBrowser::ImportResourceAsync(const std::string& filePath)
{
	futureResourceUID = App->resources->ImportThread(filePath);

	if (!isLoading)
	{
		ENGINE_LOG("Resource loaded succesfully");
		return futureResourceUID.get();
	}
	else
	{
		timer = std::make_unique<Timer>();
		timer->Start();
		ENGINE_LOG("Started loading resource");
		return NULL;
	}
}
