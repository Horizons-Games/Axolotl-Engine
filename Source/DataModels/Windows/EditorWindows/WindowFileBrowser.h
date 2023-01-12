#pragma once

#include "Windows/EditorWindows/EditorWindow.h"
#include <experimental/filesystem>
#include "ImGuiFileDialog/ImGuiFileDialog.h"

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	~WindowFileBrowser() {};
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
private:
	ImGuiFileDialog fileDialog;
	std::experimental::filesystem::path m_CurrentDirectory;
};

inline ImVec2 WindowFileBrowser::GetStartingSize() const
{
	return ImVec2(900, 250);
}

/*#include <filesystem>

class WindowFileBrowser
{
public:
	WindowFileBrowser();
	~WindowFileBrowser() {};
	ContentBrowserPanel();
	void OnImGuiRender();
private:
	std::filesystem::path m_CurrentDirectory;
};*/