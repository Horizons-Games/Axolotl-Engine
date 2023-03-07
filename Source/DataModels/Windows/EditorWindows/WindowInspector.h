#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"

#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "ImporterWindows/WindowSaveScene.h"

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
	~WindowInspector() override;

protected:
	void DrawWindowContents() override;

private:
	bool MousePosIsInWindow();
	bool WindowRightClick();

	void AddComponentMeshRenderer();
	void AddComponentMaterial();
	void AddComponentLight(LightType type);

	void DrawButtomsSaveAndLoad();

	bool showSaveScene;
	bool showLoadScene;
	std::unique_ptr<WindowLoadScene> loadScene;
	std::unique_ptr<WindowSaveScene> saveScene;

	UID lastSelectedObjectUID;
	std::vector<std::unique_ptr<ComponentWindow> > windowsForComponentsOfSelectedObject;
};