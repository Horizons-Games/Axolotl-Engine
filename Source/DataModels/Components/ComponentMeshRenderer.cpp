#pragma warning (disable: 26495)

#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "ComponentBoundingBoxes.h"
#include "Program/Program.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMesh.h"

#include "GameObject/GameObject.h"

#include "GL/glew.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
	if (IsMeshLoaded())
	{
		mesh->Unload();
	}
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	if (IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		unsigned program = App->program->GetProgram();
		const float4x4& view = App->engineCamera->GetViewMatrix();
		const float4x4& proj = App->engineCamera->GetProjectionMatrix();
		const float4x4& model =
			static_cast<ComponentTransform*>(GetOwner()
				->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

		GLint programInUse;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

		if (program != programInUse)
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ComponentMeshRenderer::DrawHighlight()
{
	if (IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		float scale = 10.1f;
		ComponentBoundingBoxes* boundingBox =
			static_cast<ComponentBoundingBoxes*>(GetOwner()->GetComponent(ComponentType::BOUNDINGBOX));
		Program* programShared = App->program->GetProgram(ProgramType::HIGHLIGHT);
		assert(programShared);
		unsigned program = programShared->GetId();
		const float4x4& view = App->engineCamera->GetViewMatrix();
		const float4x4& proj = App->engineCamera->GetProjectionMatrix();
		const float4x4& model =
			static_cast<ComponentTransform*>(GetOwner()
				->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
		GLint programInUse;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

		if (program != programInUse)
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidMesh = 0;
	std::string assetPath = "";

	if (IsMeshLoaded())
	{
		uidMesh = mesh->GetUID();
		assetPath = mesh->GetAssetsPath();
	}

	meta["meshUID"] = (UID)uidMesh;
	meta["assetPathMesh"] = assetPath.c_str();

}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh = App->resources->RequestResource<ResourceMesh>(uidMesh).lock();

	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}
	else
	{
		std::string path = meta["assetPathMesh"];
		bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
		if (resourceExists) 
		{
			uidMesh = App->resources->ImportResource(path);
			resourceMesh = App->resources->RequestResource<ResourceMesh>(uidMesh).lock();
			SetMesh(resourceMesh);
		}
	}
}

void ComponentMeshRenderer::SetMesh(const std::shared_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;

	if (IsMeshLoaded())
	{
		mesh->Load();
		ComponentBoundingBoxes* boundingBox =
			static_cast<ComponentBoundingBoxes*>(GetOwner()->GetComponent(ComponentType::BOUNDINGBOX));
		boundingBox->Encapsule(mesh->GetVertices().data(), mesh->GetNumVertices());
	}
}