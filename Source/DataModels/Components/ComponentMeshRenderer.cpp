#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "ComponentBoundingBoxes.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"

#include "GameObject/GameObject.h"

#include "Math/float3x3.h"

#include "GL/glew.h"
#include "imgui.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner, UID meshUID, UID textureUID)
	: Component(ComponentType::MESHRENDERER, active, owner), meshUID(meshUID), textureUID(textureUID)
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
}

bool ComponentMeshRenderer::Init()
{
	LoadMesh();
	LoadTexture();

	return true;
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	//lock it so it does not expire during this block
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();
	std::shared_ptr<ResourceTexture> textureAsShared = texture.lock();

	if (meshAsShared && materialAsShared && textureAsShared) //pointer not empty
	{
		unsigned program = App->program->GetProgram();
		const float4x4& view = App->engineCamera->GetViewMatrix();
		const float4x4& proj = App->engineCamera->GetProjectionMatrix();
		const float4x4& model = ((ComponentTransform*)GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

		glUseProgram(program);

		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureAsShared->GetGlTexture());
		glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

		glBindVertexArray(meshAsShared->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshAsShared->GetEBO());

		glDrawElements(GL_TRIANGLES, meshAsShared->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);
	}
}

void ComponentMeshRenderer::Display()
{
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if (meshAsShared && materialAsShared) //pointer not empty
	{

		ImGui::Text("MESH COMPONENT");
		ImGui::Dummy(ImVec2(0.0f, 2.5f));
		if (ImGui::BeginTable("##GeometryTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Number of vertices: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", meshAsShared.get()->GetNumVertices());
			ImGui::TableNextColumn();
			ImGui::Text("Number of triangles: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", meshAsShared.get()->GetNumFaces()); // faces = triangles

			ImGui::EndTable();
			ImGui::Separator();
		}
	}
}

void ComponentMeshRenderer::SetMeshUID(UID& meshUID)
{
	this->meshUID = meshUID;

	LoadMesh();
}

void ComponentMeshRenderer::SetTextureUID(UID& textureUID)
{
	this->textureUID = textureUID;

	LoadTexture();
}

void ComponentMeshRenderer::LoadMesh()
{
	mesh = std::static_pointer_cast<ResourceMesh>(App->resources->RequestResource(meshUID).lock());

	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared)
	{
		meshAsShared->Load();
		ComponentBoundingBoxes* boundingBox = ((ComponentBoundingBoxes*)GetOwner()->GetComponent(ComponentType::BOUNDINGBOX));
		boundingBox->Encapsule(meshAsShared->GetVertices().data(), meshAsShared->GetNumVertices());
	}
}

void ComponentMeshRenderer::LoadTexture()
{
	material = std::static_pointer_cast<ResourceMaterial>(App->resources->RequestResource(textureUID).lock());

	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if(materialAsShared)
	{
		materialAsShared->Load();

		texture = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(materialAsShared->GetDiffuseUID()).lock());

		std::shared_ptr<ResourceTexture> textureAsShared = texture.lock();

		if(textureAsShared) 
		{
			textureAsShared->Load();
		}

	}

}