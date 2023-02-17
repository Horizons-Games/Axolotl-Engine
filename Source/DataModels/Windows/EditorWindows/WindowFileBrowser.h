#pragma once
#include "EditorWindow.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "FontIcons/CustomFont.h"

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	~WindowFileBrowser() = default;
	void DrawWindowContents() override;
	virtual void DoThisIfOk();
	virtual void Browser();
	
protected:
	ImVec2 GetStartingSize() const override;

	bool isSave = false;
	std::string dialogName;
	std::string title;
	std::string filters;
	std::string startPath;
	std::string browserPath;

	ImGuiFileDialog fileDialogBrowser;
	ImGuiFileDialog fileDialogImporter;
	bool showFileDialog = false;
};

inline ImVec2 WindowFileBrowser::GetStartingSize() const
{
	return ImVec2(900, 250);
}
