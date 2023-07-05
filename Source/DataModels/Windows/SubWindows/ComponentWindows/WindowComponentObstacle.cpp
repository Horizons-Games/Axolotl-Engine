#include "StdAfx.h"

#include "WindowComponentObstacle.h"

#include "Components/ComponentObstacle.h"

WindowComponentObstacle::WindowComponentObstacle(ComponentObstacle* component) :
	ComponentWindow("OBSTACLE", component)
{
}

WindowComponentObstacle::~WindowComponentObstacle()
{
}

void WindowComponentObstacle::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentObstacle* asObstacle = static_cast<ComponentObstacle*>(component);

	if (asObstacle)
	{
		int obstacleType = asObstacle->GetObstacleType();
		float radius = asObstacle->GetRadius();
		float height = asObstacle->GetHeight();
		float3 size = asObstacle->GetBoxSize();
		bool mustBeDrawnGizmo = asObstacle->GetDrawGizmo();
		bool dirty = false;

		const char* shapeTypes[] = { "Cylinder", "Box" };
		if (ImGui::Combo("Obstacle type", &obstacleType, shapeTypes, IM_ARRAYSIZE(shapeTypes)))
		{
			asObstacle->SetObstacleType(static_cast<ObstacleType>(obstacleType));
		}

		if (obstacleType == ObstacleType::DT_OBSTACLE_CYLINDER)
		{
			if (ImGui::DragFloat("Cylinder radius", &radius, 0.1f, 0, inf))
			{
				asObstacle->SetRadius(radius);
			}

			if (ImGui::DragFloat("Cylinder height", &height, 0.1f, 0, inf))
			{
				asObstacle->SetHeight(height);
			}
		}
		else
		{
			ImGui::Text("Size");

			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::DragFloat("##X", &size.x, 0.1f))
			{
				dirty = true;
			}

			ImGui::SameLine();
			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::DragFloat("##Y", &size.y, 0.1f))
			{
				dirty = true;
			}

			ImGui::SameLine();
			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			if (ImGui::DragFloat("##Z", &size.z, 0.1f))
			{
				dirty = true;
			}

			if (dirty)
			{
				asObstacle->SetBoxSize(size);
			}
		}

		if (ImGui::Button("Reset Size"))
		{
			asObstacle->ResetSize();
		}

		ImGui::Text("");
		ImGui::TextWrapped("For Debug purposes only");
		if (ImGui::Checkbox("Draw Gizmos", &mustBeDrawnGizmo))
		{
			asObstacle->SetDrawGizmo(mustBeDrawnGizmo);
		}
	}
}
