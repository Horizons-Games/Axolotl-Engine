#include "Windows/EditorWindows/ImporterWindows/WindowImporter.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "FontIcons/CustomFont.cpp"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowImporter::WindowImporter()
	:EditorWindow("Importer")
{
	title = ICON_IGFD_FOLDER " Open File Dialog";
	dialogName = "Choose File";	
	filters = "Source files (*.cpp *.h *.hpp){.cpp,.h,.hpp},Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg},.md,.*";
	startPath = ".";
}
WindowImporter::~WindowImporter()
{}
void WindowImporter::DrawWindowContents()
{
	// open Dialog Simple	
	if (ImGui::Button(title))
	{
		Uint32 flags = ImGuiFileDialogFlags_Modal;
		if (isLoad)
		{
			flags |=  ImGuiFileDialogFlags_ConfirmOverwrite;
		}
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ICON_IGFD_FILE);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ICON_IGFD_FILE);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
		ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", dialogName, filters,
			startPath, ".", 1,nullptr, flags);
		
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(700, 350)))
	{
		
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				// action				
				App->resources->ImportResource(filePathName);
				
			}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}