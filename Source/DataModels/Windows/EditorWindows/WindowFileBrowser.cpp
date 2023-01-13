#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "FontIcons/CustomFont.cpp"
#include "imgui.h"

WindowFileBrowser::WindowFileBrowser() :EditorWindow("FileBrowser")
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

	if (fileDialog.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350)))
	{
		if (fileDialog.IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
		}
		fileDialog.Close();
	}
}

/*File browser c++17 (not finished)

extern const std::filesystem::path g_AssetPath = "assets";
WindowFileBrowser::WindowFileBrowser()
	: m_CurrentDirectory(g_AssetPath)
{
}
void WindowFileBrowser::OnImGuiRender()
{
	ImGui::Begin("Content Browser");

	if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	bool mouseDoubleClick = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, g_AssetPath);
		std::string filenameString = relativePath.filename().string();

		if (ImGui::Button(filenameString.c_str(), { thumbnailSize, thumbnailSize }))
		{
			if (directoryEntry.is_directory() && false)
			{
				m_CurrentDirectory /= path.filename();
			}
		}

		ImGui::Text(filenameString.c_str());

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);

	// TODO: status bar
	ImGui::End();
}*/