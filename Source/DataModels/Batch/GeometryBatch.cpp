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
	CreateVAO();
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
			if (!newComponent->GetMesh()->GetVertices().empty())
				flags |= HAS_VERTICES;

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

	glGenBuffers(1, &transforms);//
	
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		if (resourceMesh) //pointer not empty
		{
			unsigned program = App->program->GetProgram();
			const float4x4& view = App->engineCamera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->engineCamera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetComponentOwner(resourceMesh)
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			GLint programInUse;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			if (program != programInUse)
			{
				glUseProgram(program);
			}

			if (!resourceMesh->IsLoaded())
			{
			}
				//gen ebo vbo and vao buffers
				resourceMesh->Load();

			//glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, transforms);
			glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);


			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, transforms);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command) * commands.size(), &model, GL_DYNAMIC_DRAW);

			//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_DYNAMIC_DRAW);

			//send in the shader
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);


			//glBindVertexArray(0);
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

void GeometryBatch::UpdateCommands()
{
	resourceMeshIndex = 0;
	commands.clear();
	commands.reserve(components.size());//need to verify the size if it's matching with uniqueComponent
	unsigned indices_offset = 0;
	unsigned vertices_offset = 0;
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{

		//do a for for all the instaces existing
		unsigned size = resourceMeshes.size();
		Command newCommand = { resourceMesh->GetNumIndexes(), 1, indices_offset,vertices_offset/3, resourceMeshIndex };
		commands.push_back(newCommand);
		indices_offset += size;
		vertices_offset += resourceMesh->GetNumVertices();
		resourceMeshIndex++;
	}

	//send to gpu
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command) * commands.size(), &commands.front(), GL_DYNAMIC_DRAW);

}

void GeometryBatch::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//verify which data to send in buffer
	switch (flags) {
	HAS_VERTICES:
		glGenBuffers(1, &buffers->instance[0]);
		glBindBuffer(GL_ARRAY_BUFFER,buffers->instance[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);
	HAS_TEXTURE_COORDINATES:
		glGenVertexArrays(1, &buffers->instance[1]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(1);
	HAS_NORMALS:
		glGenVertexArrays(1, &buffers->instance[2]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(2);
	HAS_TANGENTS:
		glGenVertexArrays(1, &buffers->instance[3]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(3);
	}

	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	glBindVertexArray(0);

}

void GeometryBatch::UpdateVAO()
{

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
