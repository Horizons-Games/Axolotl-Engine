#include "StdAfx.h"

#include "WindowNavigation.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "FileSystem/ModuleResources.h"
#include "Windows/EditorWindows/ImporterWindows/WindowNavMeshInput.h"


WindowNavigation::WindowNavigation() :
	EditorWindow("Navigation"),
	inputNavMesh(std::make_unique<WindowNavMeshInput>()),
	nameNewNavMesh("New")
{
}

WindowNavigation::~WindowNavigation()
{
}

void WindowNavigation::DrawWindowContents()
{
	ModuleNavigation* moduleNavigation = App->GetModule<ModuleNavigation>();
	std::shared_ptr<ResourceNavMesh> navMesh = moduleNavigation->GetNavMesh();
	if (navMesh)
	{
		float cellSize = navMesh->GetCellSize();
		float cellHeight = navMesh->GetCellHeight(); 
		float agentHeight = navMesh->GetAgentHeight();
		float agentRadius = navMesh->GetAgentRadius();
		float agentMaxSlope = navMesh->GetAgentMaxSlope();
		float agentMaxClimb = navMesh->GetAgentMaxClimb();
		int regionMinSize = navMesh->GetRegionMinSize();
		int regionMergeSize = navMesh->GetRegionMergeSize();
		int partitionType = navMesh->GetPartitionType();
		int edgeMaxLen = navMesh->GetEdgeMaxLen();
		float edgeMaxError = navMesh->GetEdgeMaxError();
		int vertsPerPoly = navMesh->GetVertsPerPoly();
		int detailSampleDist = navMesh->GetDetailSampleDist();
		int detailSampleMaxError = navMesh->GetDetailSampleMaxError();
		bool keepInterResults = navMesh->GetKeepInterResults();
		int tileSize = navMesh->GetTileSize();

		ImGui::Text("Rasterization");
		if (ImGui::DragFloat("Cell Size", &cellSize, 0.05f, 0.10f, 1.0f))
		{
			navMesh->SetCellSize(cellSize);
		}
		if (ImGui::DragFloat("Cell Height", &cellHeight, 0.05f, 0.10f, 1.0f))
		{
			navMesh->SetCellHeight(cellHeight);
		}
		ImGui::Text("");

		ImGui::Text("Agent");
		if (ImGui::DragFloat("Agent Height", &agentHeight, 0.05f, 0.01f, 5.0f))
		{
			navMesh->SetAgentHeight(agentHeight);
		}
		if (ImGui::DragFloat("Agent Radius", &agentRadius, 0.05f, 0.02f, 5.0f))
		{
			navMesh->SetAgentRadius(agentRadius);
		}
		if (ImGui::DragFloat("Max Slope", &agentMaxSlope, 1.0f, 0, 60))
		{
			navMesh->SetAgentMaxSlope(agentMaxSlope);
		}
		if (ImGui::DragFloat("Step Height", &agentMaxClimb, 0.05f, 0, 5.0f))
		{
			navMesh->SetAgentMaxClimb(agentMaxClimb);
		}
		ImGui::Text("");

		ImGui::Text("Region");
		if (ImGui::DragInt("Min Region Size", &regionMinSize, 1, 0, 150))
		{
			navMesh->SetRegionMinSize(regionMinSize);
		}
		if (ImGui::DragInt("Merged Region Size", &regionMergeSize, 1, 0, 150))
		{
			navMesh->SetRegionMergeSize(regionMergeSize);
		}
		ImGui::Text("");

		ImGui::Text("Partitioning");
		if (ImGui::RadioButton("Watershed", &partitionType, 0))
		{
			navMesh->SetPartitionType(partitionType);
		}
		if (ImGui::RadioButton("Monotone", &partitionType, 1))
		{
			navMesh->SetPartitionType(partitionType);
		}
		if (ImGui::RadioButton("Layers", &partitionType, 2))
		{
			navMesh->SetPartitionType(partitionType);
		}
		ImGui::Text("");

		ImGui::Text("Polygonization");
		if (ImGui::DragInt("Max Edge Length", &edgeMaxLen, 1, 0, 50))
		{
			navMesh->SetEdgeMaxLen(edgeMaxLen);
		}
		if (ImGui::DragFloat("Max Edge Error", &edgeMaxError, 0.05f, 0.10f, 3.0f))
		{
			navMesh->SetEdgeMaxError(edgeMaxError);
		}
		if (ImGui::DragInt("Verts Per Poly", &vertsPerPoly, 1, 3, 12))
		{
			navMesh->SetVertsPerPoly(vertsPerPoly);
		}
		ImGui::Text("");

		ImGui::Text("Detail Mesh");
		if (ImGui::DragInt("Sample Distance", &detailSampleDist, 1, 0, 16))
		{
			navMesh->SetDetailSampleDist(detailSampleDist);
		}
		if (ImGui::DragInt("Max Sample Error", &detailSampleMaxError, 1, 0, 16))
		{
			navMesh->SetDetailSampleMaxError(detailSampleMaxError);
		}
		ImGui::Text("");

		if (ImGui::Checkbox("Keep Intermediate Results", &keepInterResults))
		{
			navMesh->SetKeepInterResults(keepInterResults);
		}
		ImGui::Text("");

		if (ImGui::DragInt("Tile Size", &tileSize, 8, 0, 128))
		{
			navMesh->SetTileSize(tileSize);
		}
		ImGui::Text("");

		if (ImGui::Button("Bake"))
		{
			moduleNavigation->BakeNavMesh();
		}

		ImGui::Text(("Actual NavMesh: " + navMesh->GetFileName()).c_str());
		ImGui::SameLine();
		if(ImGui::Button("X"))
		{
			moduleNavigation->SetNavMesh(nullptr);
		}
	}
	inputNavMesh->DrawWindowContents();
	ImGui::SameLine();
	DrawNavCreation();

	bool drawNavMesh = moduleNavigation->GetDrawNavMesh();
	ImGui::Text("");
	ImGui::Text("Debug");
	if (ImGui::Checkbox("Draw NavMesh", &drawNavMesh))
	{
		moduleNavigation->SetDrawNavMesh(drawNavMesh);
	}
}

void WindowNavigation::DrawNavCreation()
{

	if (ImGui::Button("Create NavMesh"))
	{
		ImGui::OpenPopup("Select Name");
		nameNewNavMesh = "NewNavMesh";
	}

	if (ImGui::BeginPopupModal("Select Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Name", &nameNewNavMesh[0], 64);
		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			std::string path = nameNewNavMesh.c_str();
			App->GetModule<ModuleResources>()->CreateDefaultResource(ResourceType::NavMesh, nameNewNavMesh.c_str());
			ImGui::CloseCurrentPopup();
			std::shared_ptr<ResourceNavMesh> resource =
				App->GetModule<ModuleResources>()->RequestResource<ResourceNavMesh>(("Assets/NavMesh/" + path + NAVMESH_EXTENSION).c_str());
			App->GetModule<ModuleNavigation>()->SetNavMesh(resource);
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}