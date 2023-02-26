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

void GeometryBatch::Draw()
{
	for (ResourceMesh* uniqueComponent : uniqueComponents)
	{
		// Generate buffer IDs
		if (indirectBuffer == 0) {
			glGenVertexArrays(1, &vao);//vertex array
			glGenBuffers(1, &indirectBuffer);//
			glGenBuffers(1, &ebo);//index buffer
			glGenBuffers(1, &vbo);//vertex buffer
		}

		// Bind buffers and VAO
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

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
				
			////vertex array
			//glBindVertexArray(uniqueComponent->GetVAO());
			//glGenVertexArrays(1,&vao);

			////index buffer
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueComponent->GetEBO());
			//glGenBuffers(1, &ebo);

			////vertex buffer
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uniqueComponent->GetVBO());
			//glGenBuffers(1, &vbo);

			//do a for for all the instaces existing
			for (unsigned int i(0); i < uniqueComponents.size(); ++i)
			{
				command[i].count = uniqueComponent->GetNumIndexes();// Number of indices in the mesh
				command[i].instanceCount = 1;
				command[i].firstIndex = 0;
				command[i].baseVertex = uniqueComponent->GetNumVertices();
				command[i].baseInstance = i;
			}

			//send to gpu
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(command),command,GL_STATIC_DRAW);//Also try with GL_DYNAMIC_DRAW
			/*
				STREAM_DRAW 
				when the data store contents will be modified once and used at most a few times.

				STATIC
				Use STATIC_DRAW when the data store contents will be modified once and used many times.

				DYNAMIC
				Use DYNAMIC_DRAW when the data store contents will be modified repeatedly and used many times.
			*/

			//send in the shader
			glBindBuffer(GL_ARRAY_BUFFER, indirectBuffer);

			//glDrawElements(GL_TRIANGLES, uniqueComponent->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);
			//use multi draw to combine with the batch method
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, uniqueComponents.size(),0);



			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);//utility ??
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

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1, &indirectBuffer);

	return true;
}
