#pragma once
#include "EditorWindow.h"

#include <future>

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "FontIcons/CustomFont.h"
#include "DataModels/Windows/EditorWindows/WindowLoading.h"

class Resource;
class Timer;

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
	void ImportResourceAsync(const std::string& filePath);
	virtual void GetResourceAfterImport();

	bool isSave = false;
	std::string dialogName;
	std::string title;
	std::string filters;
	std::string startPath;
	std::string browserPath;

	ImGuiFileDialog fileDialogBrowser;
	ImGuiFileDialog fileDialogImporter;
	
	bool showFileDialog = false;
	bool isLoading;
	std::future<std::shared_ptr<Resource>> futureResource;

private:
	void ImportResourceWithLoadingWindow();

	std::unique_ptr<WindowLoading> winLoading;
	std::unique_ptr<Timer> timer;
	std::string filePathName;
};