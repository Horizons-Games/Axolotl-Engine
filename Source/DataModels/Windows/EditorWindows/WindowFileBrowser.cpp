#include "WindowFileBrowser.h"
#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

std::future<UID> futureResourceUID;

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
	if (isLoading)
	{
		this->ImportResourceWithLoadingWindow(filePathName);
	}
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
	this->ImportResourceWithLoadingWindow(filePathName);
}

UID WindowFileBrowser::ImportResourceWithLoadingWindow(const std::string& filePath)
{
	if (!assignUID)
	{
		futureResourceUID = App->resources->ImportThread(filePath);
		assignUID = true;
	}
	
	winLoading->Draw(isLoading);
	
	if (futureResourceUID._Is_ready())
	{
		ENGINE_LOG("Loaded");
		isLoading = false;
		assignUID = false;
		return futureResourceUID.get();
	}
	else
	{
		ENGINE_LOG("Not Loaded");
		return NULL;
	}
	/*while (futureResourceUID.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
	{
		//trueVal = true;
		//winLoading->Draw(trueVal);

		//TODO: delete, just for debug
		ENGINE_LOG("Loading...");
	}*/
}
