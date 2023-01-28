#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"
// TODO: REMOVE
#include "Windows/EditorWindows/ImporterWindows/WindowLoadScene.h"
#include "ImporterWindows/WindowSaveScene.h"
// --
#include <memory>

class Model;
class GameObject;
class Component;
class ComponentCamera;
enum class LightType;

class WindowInspectorResource : public EditorWindow
{
public:
	WindowInspectorResource();
	~WindowInspectorResource();

protected:
	void DrawWindowContents() override;
	void DrawTextureOptions();
	ImVec2 GetStartingSize() const override;

private:
	bool MousePosIsInWindow();
	bool WindowRightClick();
};

inline ImVec2 WindowInspectorResource::GetStartingSize() const
{
	return ImVec2(900, 250);
}
