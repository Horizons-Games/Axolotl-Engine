#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"

#include <memory>

class Model;
class GameObject;
class Component;
class ComponentCamera;

class WindowInspector : public EditorWindow
{
public:
	WindowInspector();
	~WindowInspector();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void DrawChangeActiveComponentContent(int labelNum, Component* component);
	void DrawDeleteComponentContent(int labelNum, Component* component);
	void DrawTextureTable();
	bool MousePosIsInWindow();
	bool WindowRightClick();

	void AddComponentMeshRenderer();
	void AddComponentMaterial();
	void AddComponentLight();


	std::weak_ptr<Model> model;
};

inline ImVec2 WindowInspector::GetStartingSize() const
{
	return ImVec2(900, 250);
}
