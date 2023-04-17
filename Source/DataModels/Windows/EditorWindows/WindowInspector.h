#pragma once

#include "Application.h"
#include "EditorWindow.h"

#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

#include "Components/Component.h"

#include "Modules/ModuleScene.h"

#include "FileSystem/UniqueID.h"

#include "DataModels/GameObject/GameObject.h"

class Model;
class GameObject;
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
	void ResetSelectedGameObject();
protected:
	void DrawWindowContents() override;

private:
	void InspectSelectedGameObject();
	
	void InspectSelectedResource();
	void InitTextureImportOptions();
	void InitTextureLoadOptions();
	void DrawTextureOptions();
	
	void DrawTextureTable();
	bool MousePosIsInWindow();
	bool WindowRightClick();

	void AddComponentMeshRenderer();
	void AddComponentLight(LightType type);
	void AddComponentPlayer();
	void AddComponentRigidBody();
	void AddComponentMockState();

	GameObject* lastSelectedGameObject;
	std::weak_ptr<Resource> resource;

	//Options (Move this to another class? Probably)
	//Texture
	bool flipVertical;
	bool flipHorizontal;

	bool mipMap;
	int min;
	int mag;
	int wrapS;
	int wrapT;
	//--

	UID lastSelectedObjectUID;
	std::vector<std::unique_ptr<ComponentWindow> > windowsForComponentsOfSelectedObject;
};

inline bool WindowInspector::MousePosIsInWindow()
{
	return (ImGui::GetIO().MousePos.x > ImGui::GetWindowPos().x
		&& ImGui::GetIO().MousePos.x < (ImGui::GetWindowPos().x + ImGui::GetWindowWidth())
		&& ImGui::GetIO().MousePos.y > ImGui::GetWindowPos().y
		&& ImGui::GetIO().MousePos.y < (ImGui::GetWindowPos().y + ImGui::GetWindowHeight()));
}

inline bool WindowInspector::WindowRightClick()
{
	return (ImGui::GetIO().MouseClicked[1] && MousePosIsInWindow());
}

inline void WindowInspector::AddComponentMeshRenderer()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MESHRENDERER);
}

inline void WindowInspector::AddComponentLight(LightType type)
{
	App->scene->GetSelectedGameObject()->CreateComponentLight(type);
}

inline void WindowInspector::AddComponentPlayer()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::PLAYER);
}

inline void WindowInspector::ResetSelectedGameObject()
{
	windowsForComponentsOfSelectedObject.clear();
	lastSelectedObjectUID = 0;
}

inline void WindowInspector::AddComponentRigidBody()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::RIGIDBODY);
}

inline void WindowInspector::AddComponentMockState()
{
	App->scene->GetSelectedGameObject()->CreateComponent(ComponentType::MOCKSTATE);
}