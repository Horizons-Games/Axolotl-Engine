#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"

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

	UID lastSelectedObjectUID;
	std::vector<std::unique_ptr<ComponentWindow> > windowsForComponentsOfSelectedObject;
};