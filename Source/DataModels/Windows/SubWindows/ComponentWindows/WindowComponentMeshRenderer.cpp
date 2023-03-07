#include "WindowComponentMeshRenderer.h"

#include "DataModels/Components/ComponentMeshRenderer.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMeshInput.h"

#include "DataModels/Resources/ResourceMesh.h"

WindowComponentMeshRenderer::WindowComponentMeshRenderer(ComponentMeshRenderer* component) :
	ComponentWindow("MESH RENDERER", component), inputMesh(std::make_unique<WindowMeshInput>(component))
{
}

WindowComponentMeshRenderer::~WindowComponentMeshRenderer()
{
}

void WindowComponentMeshRenderer::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMesh> meshAsShared = asMeshRenderer->GetMesh();
		static char* meshPath = (char*)("unknown");

		if (meshAsShared)
		{
			ImGui::Text(""); //used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
			meshPath = (char*)(meshAsShared->GetLibraryPath().c_str());
		}
		else
		{
			meshPath = (char*)("unknown");
		}

		ImGui::InputText("##Mesh path", meshPath, 128);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GENERAL"))
			{
				UID draggedMeshUID = *(UID*)payload->Data; // Double pointer to keep track correctly
				//TODO this should be Asset Path of the asset not the UID (Because new filesystem cache)
				std::shared_ptr<ResourceMesh> newMesh =
					App->resources->SearchResource<ResourceMesh>(draggedMeshUID);
				//And then this should be RequestResource not SearchResource

				if (newMesh)
				{
					meshAsShared->Unload();
					asMeshRenderer->SetMesh(newMesh);
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		bool showMeshBrowser;

		meshAsShared ? showMeshBrowser = false : showMeshBrowser = true;


		if (showMeshBrowser)
		{
			inputMesh->DrawWindowContents();
		}
		else if (ImGui::Button("Remove Mesh"))
		{
			meshAsShared->Unload();
			asMeshRenderer->SetMesh(nullptr);
		}

		if (ImGui::BeginTable("##GeometryTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Number of vertices: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
				meshAsShared->GetNumVertices() : 0);
			ImGui::TableNextColumn();
			ImGui::Text("Number of triangles: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
				meshAsShared->GetNumFaces() : 0); // faces = triangles

			ImGui::EndTable();
		}
	}
}
