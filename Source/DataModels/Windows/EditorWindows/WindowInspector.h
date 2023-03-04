#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"

#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "ImporterWindows/WindowSaveScene.h"

class Model;
class GameObject;
class Component;
class ComponentCamera;
class Resource;
enum class LightType;
class ComponentWindow;

class WindowInspector : public EditorWindow
{
public:
	WindowInspector();
	~WindowInspector() override;

	void SetResource(const std::weak_ptr<Resource>& resource);

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void InspectSelectedGameObject();
	void DrawChangeActiveComponentContent(int labelNum, const std::shared_ptr<Component>& component);
	bool DrawDeleteComponentContent(int labelNum, const std::shared_ptr<Component>& component);
	
	void InspectSelectedResource();
	void InitTextureImportOptions();
	void DrawTextureOptions();
	
	void DrawTextureTable();
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
	// --
	std::shared_ptr<GameObject> lastSelectedGameObject = nullptr;
	std::weak_ptr<Resource> resource;

	//Import Options (Move this to another class? Probably)
	//Texture
	bool flipVertical;
	bool flipHorizontal;

	UID lastSelectedObjectUID;
	std::vector<std::unique_ptr<ComponentWindow> > windowsForComponentsOfSelectedObject;
};

inline ImVec2 WindowInspector::GetStartingSize() const
{
	return ImVec2(900, 250);
}
