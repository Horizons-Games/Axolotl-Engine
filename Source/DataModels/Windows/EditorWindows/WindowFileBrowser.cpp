#include "WindowFileBrowser.h"
#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowFileBrowser::WindowFileBrowser() : EditorWindow("File Browser"),
	title(ICON_IGFD_FOLDER " Import Asset"),
	dialogName("Choose File"),
	filters(".*"),
	startPath("."),
	browserPath(fileDialogBrowser.GetCurrentPath() + "Assets")
{
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".cpp",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".h",
		ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".hpp",
		ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".md",
		ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".png",
		ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".gif",
		ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr,
		ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt",
		ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "doc",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".cpp",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".h",
		ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".hpp",
		ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".md",
		ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".png",
		ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".gif",
		ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr,
		ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt",
		ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "doc",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

}

void WindowFileBrowser::DrawWindowContents()
{
	//WindowImporter
	if (ImGui::Button(title.c_str()))
	{
		Uint32 flags = ImGuiFileDialogFlags_Modal;
		if (isSave)
		{
			flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
		}
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", dialogName.c_str(), filters.c_str(), startPath.c_str(),
			"", 1, nullptr, flags);
	}
	// display
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{

		// action if OK
		if (fileDialogImporter.IsOk())
		{
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
	App->resources->ImportResource(filePathName);
}