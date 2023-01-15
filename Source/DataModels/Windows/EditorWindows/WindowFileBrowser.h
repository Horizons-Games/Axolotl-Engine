#pragma once
#include "EditorWindow.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	~WindowFileBrowser();
	void DrawWindowContents() override;
	
protected:
	ImVec2 GetStartingSize() const override;

	bool isSave = false;
	const char* dialogName;
	const char* title;
	const char* filters;
	const char* startPath;

private:
	ImGuiFileDialog fileDialogBrowser;
	ImGuiFileDialog fileDialogImporter;
	bool showFileDialog = false;
};

inline ImVec2 WindowFileBrowser::GetStartingSize() const
{
	return ImVec2(900, 250);
}
