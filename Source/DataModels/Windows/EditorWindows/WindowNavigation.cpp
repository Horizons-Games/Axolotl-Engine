#include "WindowNavigation.h"

#include "Application.h"
#include "ModuleNavigation.h"

WindowNavigation::WindowNavigation() : EditorWindow("Navigation")
{
}

WindowNavigation::~WindowNavigation()
{
}

void WindowNavigation::DrawWindowContents()
{
	ModuleNavigation* moduleNavigation = App->GetModule<ModuleNavigation>();
	std::shared_ptr<ResourceNavMesh> navMesh = moduleNavigation->GetNavMesh();

	/*ImGui::Text("Rasterization");
	ImGui::DragFloat("Cell Size", &navMesh->cellSize, 0.05f, 0.10f, 1.0f);
	ImGui::DragFloat("Cell Height", &navMesh->cellHeight, 0.05f, 0.10f, 1.0f);
	ImGui::Text("");

	ImGui::Text("Agent");
	ImGui::DragFloat("Agent Height", &navMesh->agentHeight, 0.05f, 0.01f, 5.0f);
	ImGui::DragFloat("Agent Radius", &navMesh->agentRadius, 0.05f, 0.02f, 5.0f);
	ImGui::DragFloat("Max Slope", &navMesh->agentMaxSlope, 1.0f, 0, 60);
	ImGui::DragFloat("Step Height", &navMesh->agentMaxClimb, 0.05f, 0, 5.0f);
	ImGui::Text("");

	ImGui::Text("Region");
	ImGui::DragInt("Min Region Size", &navMesh->regionMinSize, 1, 0, 150);
	ImGui::DragInt("Merged Region Size", &navMesh->regionMergeSize, 1, 0, 150);
	ImGui::Text("");

	ImGui::Text("Partitioning");
	ImGui::RadioButton("Watershed", &navMesh->partitionType, 0);
	ImGui::RadioButton("Monotone", &navMesh->partitionType, 1);
	ImGui::RadioButton("Layers", &navMesh->partitionType, 2);
	ImGui::Text("");

	ImGui::Text("Polygonization");
	ImGui::DragInt("Max Edge Length", &navMesh->edgeMaxLen, 1, 0, 50);
	ImGui::DragFloat("Max Edge Error", &navMesh->edgeMaxError, 0.05f, 0.10f, 3.0f);
	ImGui::DragInt("Verts Per Poly", &navMesh->vertsPerPoly, 1, 3, 12);
	ImGui::Text("");

	ImGui::Text("Detail Mesh");
	ImGui::DragInt("Sample Distance", &navMesh->detailSampleDist, 1, 0, 16);
	ImGui::DragInt("Max Sample Error", &navMesh->detailSampleMaxError, 1, 0, 16);
	ImGui::Text("");

	ImGui::Checkbox("Keep Intermediate Results", &navMesh->keepInterResults);
	ImGui::Text("");

	ImGui::DragInt("Tile Size", &navMesh->tileSize, 8, 0, 128);
	ImGui::Text("");*/

	if (ImGui::Button("Bake"))
	{
		moduleNavigation->BakeNavMesh();
	}

	ImGui::SameLine();

	/*if (ImGui::Button("Save"))
	{
		App->editor->modalToOpen = Modal::CREATE_NAVMESH;
	}*/
}
