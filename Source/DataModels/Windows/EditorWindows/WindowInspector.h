#pragma once

#include "EditorWindow.h"

#include <memory>

class Model;
class GameObject;
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
	void DrawTextureTable();
	bool WindowRightClick();

	std::weak_ptr<Model> model;
};

inline ImVec2 WindowInspector::GetStartingSize() const
{
	return ImVec2(900, 250);
}
