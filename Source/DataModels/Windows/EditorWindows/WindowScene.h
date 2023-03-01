#pragma once

#include "EditorWindow.h"

#include "GL/glew.h"

#include "ImGui/ImGuizmo.h"

#include "Math/float4x4.h"

#define VIEW_MANIPULATE_SIZE 128
#define VIEW_MANIPULATE_TOP_PADDING 35

class WindowScene : public EditorWindow
{
public:
	WindowScene();
	~WindowScene() override;

	ImVec2 GetStartPos() const;
	ImVec2 GetEndPos() const;
	ImVec2 GetAvailableRegion() const;

	bool isMouseInsideManipulator(float x, float y) const;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void ManageResize();
	void DrawGuizmo();

	GLuint texture;
	float currentWidth;
	float currentHeight;

	ImGuizmo::OPERATION gizmoCurrentOperation;
	ImGuizmo::MODE gizmoCurrentMode;

	bool useSnap;
	bool manipulatedLastFrame;
	float4x4 manipulatedViewMatrix;
	float3 snap;

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