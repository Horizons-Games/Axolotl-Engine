#include "WindowFileBrowser.h"
#include "FontIcons/CustomFont.cpp"
#include "imgui.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowFileBrowser::WindowFileBrowser() : EditorWindow("File Browser")
{
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);
	
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

}

WindowFileBrowser::~WindowFileBrowser() 
{
}

void WindowFileBrowser::DrawWindowContents()
{
	//WindowImporter
	if (ImGui::Button(ICON_IGFD_FOLDER" Open File Dialog"))
	{
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp,.*",
			".", 1, nullptr, ImGuiFileDialogFlags_Modal);
	}
	// display
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{

		// action if OK
		if (fileDialogImporter.IsOk())
		{
			std::string filePathName = fileDialogImporter.GetFilePathName();
			App->resources->ImportResource(filePathName);
		}

		// close
		fileDialogImporter.Close();
	}
	//End of Window importer
	
	fileDialogBrowser.OpenDialog("embedded", "Select File", ".*", "", -1, nullptr,
		ImGuiFileDialogFlags_NoDialog |
		ImGuiFileDialogFlags_DisableBookmarkMode |
		ImGuiFileDialogFlags_DisableCreateDirectoryButton |
		ImGuiFileDialogFlags_ReadOnlyFileNameField);

	fileDialogBrowser.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350));
}