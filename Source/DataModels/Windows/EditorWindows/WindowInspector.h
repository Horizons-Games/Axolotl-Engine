#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"

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
	void AddComponentMeshCollider();
	void AddComponentScript();

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