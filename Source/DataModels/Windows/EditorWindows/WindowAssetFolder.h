#pragma once
#include "EditorWindow.h"
#include <memory>

class WindowFileBrowser;

class WindowAssetFolder : public EditorWindow
{
public:
	WindowAssetFolder();
	virtual ~WindowAssetFolder() override;
	void DrawWindowContents() override;

protected:
	std::unique_ptr<WindowFileBrowser> browser;
};
