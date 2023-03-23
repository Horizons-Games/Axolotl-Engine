#include "Physics.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Geometry/Frustum.h"
#include "Math/float2.h"


bool Physics::Raycast(LineSegment ray, RaycastHit& hit)
{
	return true;
}

const LineSegment Physics::ScreenPointToRay(float2 mousePosition)
{
	// normalize the input to [-1, 1].
	const WindowScene* windowScene = App->editor->GetScene();
	ImVec2 startPosScene = windowScene->GetStartPos();
	ImVec2 endPosScene = windowScene->GetEndPos();
	LineSegment ray;

	if (!ImGuizmo::IsOver() && !windowScene->isMouseInsideManipulator(mousePosition.x, mousePosition.y))
	{
		if (mousePosition.x > startPosScene.x && mousePosition.x < endPosScene.x
			&& mousePosition.y > startPosScene.y && mousePosition.y < endPosScene.y)
		{
			mousePosition.x -= startPosScene.x;
			mousePosition.y -= startPosScene.y;

			float width = windowScene->GetAvailableRegion().x;
			float height = windowScene->GetAvailableRegion().y;

			float normalizedX = -1.0f + 2.0f * mousePosition.x / width;
			float normalizedY = 1.0f - 2.0f * mousePosition.y / height;


			ray = App->camera->GetCamera()->GetFrustum()->UnProjectLineSegment(normalizedX, normalizedY);
		}
	}
	return ray;
}
