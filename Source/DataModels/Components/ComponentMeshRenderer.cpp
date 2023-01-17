#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "ComponentBoundingBoxes.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"

#include "GameObject/GameObject.h"

#include "Math/float3x3.h"

#include "GL/glew.h"
#include "imgui.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	//lock it so it does not expire during this block
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared) //pointer not empty
	{
		if (!meshAsShared->IsLoaded())
		{
			meshAsShared->Load();
		}

		unsigned program = App->program->GetProgram();
		const float4x4& view = App->engineCamera->GetViewMatrix();
		const float4x4& proj = App->engineCamera->GetProjectionMatrix();
		const float4x4& model = ((ComponentTransform*)GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

		GLint programInUse;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

		if (program != programInUse)
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

		glBindVertexArray(meshAsShared->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshAsShared->GetEBO());

		glDrawElements(GL_TRIANGLES, meshAsShared->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ComponentMeshRenderer::Display()
{
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (ImGui::CollapsingHeader("MESH RENDERER", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static char* meshPath = (char*)("unknown");

		if (meshAsShared)
			meshPath = (char*)(meshAsShared->GetLibraryPath().c_str());

		ImGui::InputText("##Mesh path", meshPath, 128);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GENERAL"))
			{
				UID draggedMeshUID = *(UID*)payload->Data; // Double pointer to keep track correctly

				std::shared_ptr<ResourceMesh> newMesh =
					App->resources->RequestResource<ResourceMesh>(draggedMeshUID).lock();

				if (newMesh)
					SetMesh(newMesh);
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();

		if (ImGui::Button("Remove Current Mesh"))
		{
			mesh = std::weak_ptr<ResourceMesh>();
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

	ImGui::Separator();
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	UID uidMesh = 0;

	if(meshAsShared)
	{
		uidMesh = meshAsShared->GetUID();
	}

	meta["meshUID"] = (UID)uidMesh;

	//meta["mesh"] = (std::weak_ptr<ResourceMesh>) mesh;
}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	UID uidMesh = meta["meshUID"];

	SetMesh(App->resources->RequestResource<ResourceMesh>(uidMesh).lock());
}

void ComponentMeshRenderer::SetMesh(const std::weak_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared)
	{
		meshAsShared->Load();
		ComponentBoundingBoxes* boundingBox = ((ComponentBoundingBoxes*)GetOwner()->GetComponent(ComponentType::BOUNDINGBOX));
		boundingBox->Encapsule(meshAsShared->GetVertices().data(), meshAsShared->GetNumVertices());
	}
}