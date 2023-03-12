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
	buffers = new ResourceMesh(0,"","","");
	
	CreateVBO();
	CreateEBO();
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

void GeometryBatch::CreateVBO()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		//position			//uv				//normal		
		unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3);
		//tangents
		if (resourceMesh->GetTangents().size() != 0)
		{
			vertexSize += sizeof(float) * 3;
		}
		//unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2);
		GLuint bufferSize = vertexSize * resourceMesh->GetNumVertices();

		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		GLuint positionSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();

		glBufferSubData(GL_ARRAY_BUFFER, 0, positionSize, &(resourceMesh->GetVertices()[0]));

		GLuint uvOffset = positionSize;
		GLuint uvSize = sizeof(float) * 2 * resourceMesh->GetNumVertices();

		float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

		for (unsigned int i = 0; i < resourceMesh->GetNumVertices(); ++i)
		{
			uvs[i] = float2(resourceMesh->GetTextureCoords()[i].x, resourceMesh->GetTextureCoords()[i].y);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		unsigned normalsOffset = positionSize + uvSize;
		unsigned normalsSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();
		glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, &resourceMesh->GetNormals()[0]);

		if (resourceMesh->GetTangents().size() != 0)
		{
			unsigned tangentsOffset = positionSize + uvSize + normalsSize;
			unsigned tangentsSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();
			glBufferSubData(GL_ARRAY_BUFFER, tangentsOffset, tangentsSize, &resourceMesh->GetTangents()[0]);
		}
	}
	

}

void GeometryBatch::CreateEBO()
{
	for (ResourceMesh* resourceMesh : resourceMeshes)
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		GLuint indexSize = sizeof(GLuint) * resourceMesh->GetNumFaces() * 3;

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

		GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

		for (unsigned int i = 0; i < resourceMesh->GetNumFaces(); ++i)
		{
			assert(resourceMesh->GetFacesIndices()[i].size() == 3); // note: assume triangles = 3 indices per face
			*(indices++) = resourceMesh->GetFacesIndices()[i][0];
			*(indices++) = resourceMesh->GetFacesIndices()[i][1];
			*(indices++) = resourceMesh->GetFacesIndices()[i][2];
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
}


void GeometryBatch::BindBatch()
{
	const GLuint bindingPointCamera = 0;
	const GLuint bindingPointModel = 10;
	//model(transforms)
	glGenBuffers(1, &transforms);//
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms);
	const int bufferSize = resourceMeshes.size() * sizeof(float4x4);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	//camera
	GLuint cameraUniformBlockID;
	glGenBuffers(1, &cameraUniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlockID);
	
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
				//gen ebo vbo and vao buffers
				//resourceMesh->Load();
			}


			float4x4* modelMatrices = (float4x4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);


			//binding the uniform camera
			GLint cameraUniformBlockIndex = glGetUniformBlockIndex(program, "Camera");
			glUniformBlockBinding(program, cameraUniformBlockIndex, bindingPointCamera);

			glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), (const float*)&proj);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), (const float*)&view);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointCamera, cameraUniformBlockID);

			//glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
			//glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
			//glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);


			//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, transforms);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Command) * commands.size(), &model, GL_DYNAMIC_DRAW);

			//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_DYNAMIC_DRAW);

			//send in the shader
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

			//glBindVertexArray(0);
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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

	//vertices
	glGenBuffers(1, &buffers->instance[0]);
	glBindBuffer(GL_ARRAY_BUFFER,buffers->instance[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	//texture
	glGenVertexArrays(1, &buffers->instance[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	//normals
	glGenVertexArrays(1, &buffers->instance[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(2);
	//tangents
	glGenVertexArrays(1, &buffers->instance[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	glBindVertexArray(0);

}

void GeometryBatch::UpdateVAO()
{
	glBindVertexArray(vao);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[0]);
	glBufferData(GL_ARRAY_BUFFER, buffers->GetNumVertices() * sizeof(float), &buffers->GetVertices(), GL_DYNAMIC_DRAW);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[1]);
	glBufferData(GL_ARRAY_BUFFER, buffers->GetNumVertices() * sizeof(float), &buffers->GetNormals(), GL_DYNAMIC_DRAW);

	//texture
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[2]);
	glBufferData(GL_ARRAY_BUFFER, buffers->GetNumVertices() * sizeof(float), &buffers->GetTextureCoords(), GL_DYNAMIC_DRAW);

	//tangents
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[3]);
	glBufferData(GL_ARRAY_BUFFER, buffers->GetNumVertices() * sizeof(float), &buffers->GetTangents(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
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
