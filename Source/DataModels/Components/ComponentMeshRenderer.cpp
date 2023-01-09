#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"

#include "GameObject/GameObject.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"

#include "GL/glew.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESH, active, owner)
{
	mesh = std::make_unique<ResourceMesh>(UniqueID::GenerateUID(), "", "");
	texture = std::make_unique<ResourceTexture>(UniqueID::GenerateUID(), "", "");
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
}

void ComponentMeshRenderer::Update()
{
	if (GetActive())
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
	glBindTexture(GL_TEXTURE_2D, texture->GetUID());
	glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

	glBindVertexArray(mesh->GetVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

	glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() / 3, GL_UNSIGNED_INT, nullptr);
}