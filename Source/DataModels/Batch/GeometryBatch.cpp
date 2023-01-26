#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Resources/ResourceMesh.h"

#include <gl/glew.h>

GeometryBatch::GeometryBatch()
{}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	uniqueComponents.clear();
}

void GeometryBatch::AddComponentMeshRenderer(const std::shared_ptr<ComponentMeshRenderer>& newComponent)
{
	if (newComponent != nullptr)
	{
		AddUniqueComponent(newComponent->GetMesh());
		components.push_back(newComponent);
	}
}

void GeometryBatch::Draw()
{
	for (std::weak_ptr<ResourceMesh>& uniqueComponent : uniqueComponents)
	{
		//lock it so it does not expire during this block
		std::shared_ptr<ResourceMesh> meshAsShared = uniqueComponent.lock();

		if (meshAsShared) //pointer not empty
		{
			if (!meshAsShared->IsLoaded())
			{
				meshAsShared->Load();
			}

			unsigned program = App->program->GetProgram();
			const float4x4& view = App->engineCamera->GetViewMatrix();
			const float4x4& proj = App->engineCamera->GetProjectionMatrix();
			const float4x4& model =
				std::static_pointer_cast<ComponentTransform>(GetComponentOwner(uniqueComponent)
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

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
}

const std::weak_ptr<ResourceMesh>& GeometryBatch::GetMesh(const UID meshUID) const
{
	for (std::shared_ptr<ComponentMeshRenderer> component : components)
	{
		if (component->GetMeshUID() == meshUID)
		{
			return component->GetMesh();
		}
	}

	return std::weak_ptr<ResourceMesh>();
}

void GeometryBatch::AddUniqueComponent(const std::weak_ptr<ResourceMesh>& resourceMesh)
{
	if (isUniqueResourceMesh(resourceMesh))
	{
		uniqueComponents.push_back(resourceMesh);
	}
}

const std::shared_ptr<GameObject>& GeometryBatch::GetComponentOwner(const std::weak_ptr<ResourceMesh>& resourceMesh)
{
	for (std::shared_ptr<ComponentMeshRenderer>& component : components)
	{
		if (component->GetMesh().lock() == resourceMesh.lock())
		{
			return component->GetOwner().lock();
		}
	}

	return nullptr;
}

bool GeometryBatch::isUniqueResourceMesh(const std::weak_ptr<ResourceMesh>& resourceMesh)
{
	if (resourceMesh.expired())
	{
		return false;
	}

	std::shared_ptr<ResourceMesh> meshAsShared = resourceMesh.lock();

	for (std::weak_ptr<ResourceMesh> uniqueComponent : uniqueComponents)
	{
		if (uniqueComponent.lock() == meshAsShared)
		{
			return false;
		}
	}

	return true;
}