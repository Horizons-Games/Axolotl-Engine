#include "WindowFileBrowser.h"
#include "FontIcons/CustomFont.cpp"
#include "imgui.h"

WindowFileBrowser::WindowFileBrowser() : EditorWindow("File Browser")
{
	fileDialog.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialog.SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialog.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
	fileDialog.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	fileDialog.SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	fileDialog.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	fileDialog.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

}

WindowFileBrowser::~WindowFileBrowser() 
{
}

void WindowFileBrowser::DrawWindowContents()
{
	//WindowImporter
	if (ImGui::Button(ICON_IGFD_FOLDER" Open File Dialog"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp,.*",
			".", 1, nullptr, ImGuiFileDialogFlags_Modal);
	}
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{

		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// TODO: action

		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	//End of Window importer
	
	fileDialog.OpenDialog("embedded", "Select File", ".*", "", -1, nullptr,
		ImGuiFileDialogFlags_NoDialog |
		ImGuiFileDialogFlags_DisableBookmarkMode |
		ImGuiFileDialogFlags_DisableCreateDirectoryButton |
		ImGuiFileDialogFlags_ReadOnlyFileNameField);

	fileDialog.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350));
}