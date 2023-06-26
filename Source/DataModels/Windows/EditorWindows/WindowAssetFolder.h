#pragma once
#include "EditorWindow.h"
#include "Resources/Resource.h"

class WindowFileBrowser;

class WindowAssetFolder : public EditorWindow
{
public:
	WindowAssetFolder();
	virtual ~WindowAssetFolder() override;
	void DrawWindowContents() override;

protected:
	std::unique_ptr<WindowFileBrowser> browser;
	ResourceType type;
	std::string name;
};
