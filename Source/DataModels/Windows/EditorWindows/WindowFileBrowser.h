#pragma once
#include "EditorWindow.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "FontIcons/CustomFont.h"

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	virtual ~WindowFileBrowser() override;
	void DrawWindowContents() override;
	void SaveAsWindow(bool& isSaving);
	virtual void DoThisIfOk();
	virtual void Browser();
	
protected:

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

