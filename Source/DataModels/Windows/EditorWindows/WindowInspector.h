#pragma once

#include "EditorWindow.h"

#include "FileSystem/UID.h"

class Model;
class GameObject;
class Component;
class ComponentCamera;
class Resource;
enum class LightType;
enum class AreaType;
class ComponentWindow;

struct AddComponentAction;

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
	void AddComponentLight(LightType type, AreaType areaType);
	void AddComponentPlayer();
	void AddComponentPlayerInput();
	void AddComponentCameraSample();
	void AddComponentAnimation();

	void AddComponentRigidBody();
	void AddComponentAudioSource();
	void AddComponentAudioListener();
	void AddComponentMeshCollider();
	void AddComponentScript();
	void AddComponentParticle();
	void AddComponentBreakable();
	void AddComponentTrail();
	void AddComponentLine();

	GameObject* lastSelectedGameObject;
	std::weak_ptr<Resource> resource;

	// Options (Move this to another class? Probably)
	// Texture
	bool flipVertical;
	bool flipHorizontal;

	bool mipMap;
	int min;
	int mag;
	int wrapS;
	int wrapT;
	//--

	UID lastSelectedObjectUID;
	std::vector<std::unique_ptr<ComponentWindow>> windowsForComponentsOfSelectedObject;
	std::vector<AddComponentAction> actions;
};

inline bool WindowInspector::MousePosIsInWindow()
{
	return (ImGui::GetIO().MousePos.x > ImGui::GetWindowPos().x &&
			ImGui::GetIO().MousePos.x < (ImGui::GetWindowPos().x + ImGui::GetWindowWidth()) &&
			ImGui::GetIO().MousePos.y > ImGui::GetWindowPos().y &&
			ImGui::GetIO().MousePos.y < (ImGui::GetWindowPos().y + ImGui::GetWindowHeight()));
}

inline bool WindowInspector::WindowRightClick()
{
	return (ImGui::GetIO().MouseClicked[1] && MousePosIsInWindow());
}
