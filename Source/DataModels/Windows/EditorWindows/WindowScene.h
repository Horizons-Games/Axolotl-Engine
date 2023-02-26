#pragma once

#include "EditorWindow.h"

#include "GL/glew.h"

#include "ImGui/ImGuizmo.h"

class WindowScene : public EditorWindow
{
public:
	WindowScene();
	~WindowScene();

	ImVec2 GetStartPos() const;
	ImVec2 GetEndPos() const;
	ImVec2 GetAvailableRegion() const;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void ManageResize();
	void DrawGuizmo();

	GLuint texture = 0;
	float currentWidth = 0;
	float currentHeight = 0;

	ImGuizmo::OPERATION gizmoCurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE gizmoCurrentMode = ImGuizmo::LOCAL;

	ImVec2 availableRegion;
	ImVec2 viewportBounds[2]; // [0] minViewport, [1] maxViewport
};

inline ImVec2 WindowScene::GetStartingSize() const
{
	return ImVec2(900, 250);
}

inline ImVec2 WindowScene::GetStartPos() const
{
	return viewportBounds[0];
}	

inline ImVec2 WindowScene::GetEndPos() const
{
	return viewportBounds[1];
}

inline ImVec2 WindowScene::GetAvailableRegion() const
{
	return availableRegion;
}