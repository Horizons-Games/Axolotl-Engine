#pragma once
#include "EditorWindow.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	~WindowFileBrowser();
	
protected:
	void DrawWindowContents() override;
	ImVec2 GetStartingSize() const override;

private:
	ImGuiFileDialog fileDialogBrowser;
	ImGuiFileDialog fileDialogImporter;
	bool showFileDialog = false;
};

inline ImVec2 WindowFileBrowser::GetStartingSize() const
{
	return ImVec2(900, 250);
}
