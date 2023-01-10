#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceMesh.h"

#include "GameObject/GameObject.h"

#include "Math/float3x3.h"

#include "GL/glew.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner, UID& meshUID, UID& textureUID)
	: Component(ComponentType::MESH, active, owner), meshUID(meshUID), textureUID(textureUID)
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
}

void ComponentMeshRenderer::Init()
{
	LoadMesh();
}

void ComponentMeshRenderer::Update()
{
	if (GetActive() && IsMeshLoaded())
	{
		Draw();
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
	glBindTexture(GL_TEXTURE_2D, textureUID);
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

	glBindVertexArray(mesh->GetVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

	glDrawElements(GL_TRIANGLES, mesh->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
}

void ComponentMeshRenderer::SetMeshUID(UID& meshUID)
{
	this->meshUID = meshUID;

	LoadMesh();
}

void ComponentMeshRenderer::LoadMesh()
{
	mesh = std::static_pointer_cast<ResourceMesh>(App->resources->RequestResource(meshUID));
}