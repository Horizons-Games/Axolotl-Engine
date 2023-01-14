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

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner)
{
}

// This Constructor will disappear
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
	if (GetActive() && meshUID != 0LL && textureUID != 0LL)
	{
		Draw();
	}
}

void ComponentMeshRenderer::Display()
{
	ImGui::Text("MESH RENDERER COMPONENT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("##GeometryTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Number of vertices: ");
		ImGui::TableNextColumn();
		//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", mesh.get()->GetNumVertices());
		ImGui::TableNextColumn();
		ImGui::Text("Number of triangles: ");
		ImGui::TableNextColumn();
		//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", mesh.get()->GetNumFaces()); // faces = triangles

		ImGui::EndTable();
		ImGui::Separator();
	}

}

void ComponentMeshRenderer::Draw()
{
	unsigned program = App->program->GetProgram();
	const float4x4& view = App->engineCamera->GetViewMatrix();
	const float4x4& proj = App->engineCamera->GetProjectionMatrix();
	const float4x4& model = ((ComponentTransform*) GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

	glBindVertexArray(mesh->GetVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

	glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);
}

void ComponentMeshRenderer::Display()
{
	ImGui::Text("MESH COMPONENT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("##GeometryTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Number of vertices: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", mesh.get()->GetNumVertices());
		ImGui::TableNextColumn();
		ImGui::Text("Number of triangles: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", mesh.get()->GetNumFaces()); // faces = triangles

		ImGui::EndTable();
		ImGui::Separator();
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
	if (meshUID != 0LL)
	{
		mesh = std::static_pointer_cast<ResourceMesh>(App->resources->RequestResource(meshUID));
		ComponentBoundingBoxes* boundingBox = ((ComponentBoundingBoxes*)GetOwner()->GetComponent(ComponentType::BOUNDINGBOX));
		mesh->Load();
		boundingBox->Encapsule(mesh->GetVertices().data(), mesh->GetNumVertices());
	}
}

void ComponentMeshRenderer::LoadTexture()
{	
	if (textureUID != 0LL)
	{
		texture = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(textureUID));
		texture->Load();
	}
}