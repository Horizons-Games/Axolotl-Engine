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
	resourceMeshes.clear();
	CleanUp();
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent != nullptr)
	{
		if (components.empty())
		{
			if(!newComponent->GetMesh()->GetNormals().empty())
				flags |= HAS_NORMALS;

			if (!newComponent->GetMesh()->GetTextureCoords().empty())
				flags |= HAS_TEXTURE_COORDINATES;

			if (!newComponent->GetMesh()->GetTangents().empty())
				flags |= HAS_TANGENTS;
		}

		AddUniqueComponent(newComponent->GetMesh().get());
		components.push_back(newComponent);
	}
}

void GeometryBatch::BindBatch()
{
	unsigned int resourceMesheIndex = 0;
	command.clear();
	command.reserve(components.size());//need to verify the size if it's matching with uniqueComponent
	for (ResourceMesh* resourceMeshe : resourceMeshes)
	{
		if (resourceMeshe) //pointer not empty
		{
			if (!resourceMeshe->IsLoaded())
			{
				//gen ebo vbo and vao buffers
				resourceMeshe->Load();
				if (indirectBuffer == 0) {
					glGenBuffers(1, &indirectBuffer);//
				}
			}

			unsigned program = App->program->GetProgram();
			const float4x4& view = App->engineCamera->GetViewMatrix();
			const float4x4& proj = App->engineCamera->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetComponentOwner(resourceMeshe)
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
				
			//do a for for all the instaces existing
			command[resourceMesheIndex].count = resourceMeshe->GetNumIndexes();// Number of indices in the mesh
			command[resourceMesheIndex].baseVertex = resourceMeshe->GetNumVertices();
			command[resourceMesheIndex].baseInstance = resourceMesheIndex;
			resourceMesheIndex++;

			//send to gpu
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(command),&command[0], GL_STATIC_DRAW);
			//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(command), command, GL_DYNAMIC_DRAW);

			//send in the shader
			glBindBuffer(GL_ARRAY_BUFFER, indirectBuffer);


			//glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

void GeometryBatch::AddUniqueComponent(ResourceMesh* resourceMesh)
{
	if (isUniqueResourceMesh(resourceMesh))
	{
		resourceMeshes.push_back(resourceMesh);
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
	for (ResourceMesh* resourceMeshe : resourceMeshes)
	{
		if (resourceMeshe == resourceMesh)
		{
			return false;
		}
	}

	return true;
}

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1, &indirectBuffer);

	return true;
}
