#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

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
	if (newComponent)
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

		resourceMeshes.insert(newComponent->GetMesh().get());
		components.push_back(newComponent);
	}
}

void GeometryBatch::BindBatch()
{
	//command.reserve(components.size());//need to verify the size if it's matching with uniqueComponent
	unsigned int instanceIndex = 0;
	commands.clear();

	unsigned program = App->program->GetProgram();
	
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		if (resourceMesh) //pointer not empty
		{
			if (!resourceMesh->IsLoaded())
			{
				//gen ebo vbo and vao buffers
				resourceMesh->Load();
				if (indirectBuffer == 0) {
					glGenBuffers(1, &indirectBuffer);
				}
			}

			const float4x4& model =
				static_cast<ComponentTransform*>(GetComponentOwner(resourceMesh)
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			GLint programInUse;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			if (program != programInUse)
			{
				glUseProgram(program);
			}

			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
				
			//do a for for all the instaces existing
			Command newCommand = { resourceMesh->GetNumIndexes(), 1, 0, resourceMesh->GetNumVertices(), instanceIndex };
			commands.push_back(newCommand);
			instanceIndex++;

			//send to gpu
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), &commands[0], GL_STATIC_DRAW);
			//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_DYNAMIC_DRAW);

			//send in the shader
			glBindBuffer(GL_ARRAY_BUFFER, indirectBuffer);


			//glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
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

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1, &indirectBuffer);

	return true;
}
