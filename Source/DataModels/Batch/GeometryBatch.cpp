#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Resources/ResourceMesh.h"
#include "DataModels/Batch/BatchFlags.h"

#include <gl/glew.h>



GeometryBatch::GeometryBatch()
{
}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	uniqueComponents.clear();
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent != nullptr)
	{
		if (components.empty())
		{
			if(!newComponent->GetMesh()->GetNormals().empty())
				flags << HAS_NORMALS;

			if (!newComponent->GetMesh()->GetTextureCoords().empty())
				flags << HAS_TEXTURE_COORDINATES;

			if (!newComponent->GetMesh()->GetTangents().empty())
				flags << HAS_TANGENTS;
		}

		AddUniqueComponent(newComponent->GetMesh().get());
		components.push_back(newComponent);
	}
}

void GeometryBatch::Draw()
{
	for (ResourceMesh* uniqueComponent : uniqueComponents)
	{

		if (uniqueComponent) //pointer not empty
		{
			if (!uniqueComponent->IsLoaded())
			{
				uniqueComponent->Load();
			}

			unsigned program = App->program->GetProgram();
			const float4x4& view = App->engineCamera->GetViewMatrix();
			const float4x4& proj = App->engineCamera->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetComponentOwner(uniqueComponent)
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

			glBindVertexArray(uniqueComponent->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueComponent->GetEBO());

			glDrawElements(GL_TRIANGLES, uniqueComponent->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

void GeometryBatch::AddUniqueComponent(ResourceMesh* resourceMesh)
{
	if (isUniqueResourceMesh(resourceMesh))
	{
		uniqueComponents.push_back(resourceMesh);
	}
}

const GameObject* GeometryBatch::GetComponentOwner(const ResourceMesh* resourceMesh)
{
	for (ComponentMeshRenderer* component : components)
	{
		if (component->GetMesh().get() == resourceMesh)
		{
			return component->GetOwner();
		}
	}

	return nullptr;
}

bool GeometryBatch::isUniqueResourceMesh(const ResourceMesh* resourceMesh)
{
	for (ResourceMesh* uniqueComponent : uniqueComponents)
	{
		if (uniqueComponent == resourceMesh)
		{
			return false;
		}
	}

	return true;
}