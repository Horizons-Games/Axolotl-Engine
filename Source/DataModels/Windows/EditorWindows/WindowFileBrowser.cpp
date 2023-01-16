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
			std::string filePathName = fileDialogImporter.GetFilePathName();
			std::string filePathName2 = fileDialogImporter.GetCurrentFileName();
			filePathName += filePathName2;
			DoThisIfOk();
		}

		// close
		fileDialogImporter.Close();
	}
	//End of Window importer
	
	if(title == ICON_IGFD_FOLDER " Open File Dialog")
	{
		fileDialogBrowser.OpenDialog("embedded", "Select File", ".*", "", -1, nullptr,
			ImGuiFileDialogFlags_NoDialog |
			ImGuiFileDialogFlags_DisableBookmarkMode |
			ImGuiFileDialogFlags_DisableCreateDirectoryButton |
			ImGuiFileDialogFlags_ReadOnlyFileNameField);

		fileDialogBrowser.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350));
	}
}

void WindowFileBrowser::DoThisIfOk()
{
	std::string filePathName = fileDialogImporter.GetFilePathName();
	App->resources->ImportResource(filePathName);
}