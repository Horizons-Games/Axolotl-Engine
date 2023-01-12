#include "Windows/EditorWindows/WindowFileBrowser.h"
#include "FontIcons/CustomFont.cpp"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imgui.h"
#include <limits>

WindowFileBrowser::WindowFileBrowser() :EditorWindow("FileBrowser")
{
}
void WindowFileBrowser::DrawWindowContents()
{
	if (ImGui::Button(ICON_IGFD_FOLDER" Open File Dialog"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("FileBrowserPanel", "File Browser", ".cpp,.h,.hpp,.*",
			".", 1, nullptr);
	}

	if (ImGuiFileDialog::Instance()->Display("FileBrowserPanel"))
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