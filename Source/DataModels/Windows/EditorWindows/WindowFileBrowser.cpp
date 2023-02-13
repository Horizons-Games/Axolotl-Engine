#include "WindowFileBrowser.h"
#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

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
			trueVal = true;
		}

		// close
		fileDialogImporter.Close();
	}
	//End of Window importer
	
	if(title == ICON_IGFD_FOLDER " Import Asset")
	{
		Browser();
	}
	if (trueVal)
	{
		winLoading->Draw(trueVal);
		++i;
		if (1 < i) 
		{
			DoThisIfOk();
			i = 0;
		}
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
	std::string filePathName = fileDialogImporter.GetFilePathName();
	this->ImportResourceWithLoadingWindow(filePathName);
}

UID WindowFileBrowser::ImportResourceWithLoadingWindow(const std::string& filePath)
{
	std::future<UID> futureResourceUID = App->resources->ImportThread(filePath);
	//int k = 0;
	//while (k<10000)
	while (futureResourceUID.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
	{
		//trueVal = true;
		//winLoading->Draw(trueVal);
		//++k;
		//TODO: delete, just for debug
		ENGINE_LOG("Loading...");
	}

	trueVal = false;
	return futureResourceUID.get();
}
