#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"
// TODO: REMOVE
#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "ImporterWindows/WindowSaveScene.h"
// --

class Model;
class GameObject;
class Component;
class ComponentCamera;
enum class LightType;
class ComponentWindow;

class WindowInspector : public EditorWindow
{
public:
	WindowInspector();
	~WindowInspector();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void DrawTextureTable();
	bool MousePosIsInWindow();
	bool WindowRightClick();

	void AddComponentMeshRenderer();
	void AddComponentMaterial();
	void AddComponentLight(LightType type);

	// TODO: REMOVE
	bool showSaveScene = true;
	bool showLoadScene = true;
	void DrawButtomsSaveAndLoad();
	std::unique_ptr<WindowLoadScene> loadScene;
	std::unique_ptr<WindowSaveScene> saveScene;
	// --

	UID lastSelectedObjectUID = 0;
	std::vector<std::unique_ptr<ComponentWindow> > windowsForComponentsOfSelectedObject;
};

inline ImVec2 WindowInspector::GetStartingSize() const
{
	return ImVec2(900, 250);
}
