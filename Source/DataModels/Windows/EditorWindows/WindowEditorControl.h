#pragma once
#include "EditorWindow.h"
#include "Windows/SubWindows/SubWindow.h"
#include "ModuleScene.h"
// TODO: REMOVE
#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "Windows/EditorWindows/ImporterWindows/WindowSaveScene.h"
// --
#include <vector>
#include <memory>

class GameObject;

class WindowEditorControl : public EditorWindow
{
public:
	WindowEditorControl();
	~WindowEditorControl();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
private:
	// TODO: REMOVE
	bool showSaveScene = true;
	bool showLoadScene = true;
	void DrawButtomsSaveAndLoad();
	std::unique_ptr<WindowLoadScene> loadScene;
	std::unique_ptr<WindowSaveScene> saveScene;
	// --
};

inline ImVec2 WindowEditorControl::GetStartingSize() const
{
	return ImVec2(900, 250);
}
