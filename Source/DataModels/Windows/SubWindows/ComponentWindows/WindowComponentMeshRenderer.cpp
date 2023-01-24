#include "WindowComponentMeshRenderer.h"

#include "DataModels/Components/ComponentMeshRenderer.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMeshInput.h"

#include "DataModels/Resources/ResourceMesh.h"

WindowComponentMeshRenderer::WindowComponentMeshRenderer(const std::weak_ptr<ComponentMeshRenderer>& component) :
	ComponentWindow("MESH RENDERER", component)
{
	inputMesh = std::make_unique<WindowMeshInput>(component);
}

void WindowComponentMeshRenderer::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentMeshRenderer> asSharedMeshRenderer =
		std::dynamic_pointer_cast<ComponentMeshRenderer>(this->component.lock());

	if (asSharedMeshRenderer)
	{
		std::shared_ptr<ResourceMesh> meshAsShared = asSharedMeshRenderer->GetMesh().lock();
		if (meshAsShared)
		{
			static char* meshPath = (char*)("unknown");

			if (meshAsShared)
				meshPath = (char*)(meshAsShared->GetLibraryPath().c_str());
			else
				meshPath = (char*)("unknown");

			ImGui::InputText("##Mesh path", meshPath, 128);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GENERAL"))
				{
					UID draggedMeshUID = *(UID*)payload->Data; // Double pointer to keep track correctly

					std::shared_ptr<ResourceMesh> newMesh =
						App->resources->RequestResource<ResourceMesh>(draggedMeshUID).lock();

					if (newMesh)
					{
						meshAsShared->Unload();
						asSharedMeshRenderer->SetMesh(newMesh);
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
				asSharedMeshRenderer->SetMesh(std::weak_ptr<ResourceMesh>());
			}

			if (ImGui::BeginTable("##GeometryTable", 2))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Number of vertices: ");
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
					meshAsShared.get()->GetNumVertices() : 0);
				ImGui::TableNextColumn();
				ImGui::Text("Number of triangles: ");
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
					meshAsShared.get()->GetNumFaces() : 0); // faces = triangles

				ImGui::EndTable();
			}
		}
	}
}
