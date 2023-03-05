#pragma once
#include "EditorWindow.h"

#include <future>

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "FontIcons/CustomFont.h"
#include "DataModels/Windows/EditorWindows/WindowLoading.h"

class Timer;

class WindowFileBrowser : public EditorWindow
{
public:
	WindowFileBrowser();
	virtual ~WindowFileBrowser() override;
	void DrawWindowContents() override;
	virtual void DoThisIfOk();
	virtual void Browser();
	
protected:
	unsigned long long ImportResourceAsync(const std::string& filePath);

	bool isSave = false;
	std::string dialogName;
	std::string title;
	std::string filters;
	std::string startPath;
	std::string browserPath;

	ImGuiFileDialog fileDialogBrowser;
	ImGuiFileDialog fileDialogImporter;
	bool showFileDialog = false;

private:
	void ImportResourceWithLoadingWindow();

	std::unique_ptr<WindowLoading> winLoading;
	std::future<unsigned long long> futureResourceUID;
	std::unique_ptr<Timer> timer;
	bool isLoading;
	std::string filePathName;
};