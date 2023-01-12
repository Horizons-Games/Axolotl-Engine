#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "FontIcons/CustomFont.cpp"
//#include "FileSystem/ModuleFileSystem.h"
#include "imgui.h"

WindowFileBrowser::WindowFileBrowser() :EditorWindow("FileBrowser")
{
	
}
void WindowFileBrowser::DrawWindowContents()
{
	//ModuleFileSystem fileSys;
	//std::vector<std::string> files = fileSys.listFiles("C:/Users/pedrog3/Documents/GitHub/Engine/Source");

	

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

/*extern const std::filesystem::path g_AssetPath = "assets";
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