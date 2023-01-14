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

	return true;
}

void ComponentMeshRenderer::Update()
{
	if (GetActive() && IsMeshLoaded())
	{
		Draw();
	}
}

void ComponentMeshRenderer::Display()
{
}

void ComponentMeshRenderer::Draw()
{
	//lock it so it does not expire during this block
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared) //pointer not empty
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
		glBindTexture(GL_TEXTURE_2D, textureUID);
		glUniform1i(glGetUniformLocation(program, "diffuse"), 0);

		glBindVertexArray(meshAsShared->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshAsShared->GetEBO());

		glDrawElements(GL_TRIANGLES, meshAsShared->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		LoadMesh();
	}
}

void ComponentMeshRenderer::SetMeshUID(UID& meshUID)
{
	this->meshUID = meshUID;

	LoadMesh();
}

void ComponentMeshRenderer::LoadMesh()
{
	mesh = std::static_pointer_cast<ResourceMesh>(App->resources->RequestResource(meshUID).lock());
}