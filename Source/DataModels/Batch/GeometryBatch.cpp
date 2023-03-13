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
#include <assert.h>

GeometryBatch::GeometryBatch()
{
	//TODO complete
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	resourceMeshes.clear();
	CleanUp();
}

void GeometryBatch::CalculateVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
							//position			//uv				//normal		
	unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3);
	
	//tangents
	if (flags && HAS_TANGENTS) //TODO see how we do that
	{
		vertexSize += sizeof(float) * 3;
	}

	GLuint bufferSize = vertexSize * numTotalVertices;

	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

	int spaceUsed = 0;

	for (auto aaa : resourceMeshes)
	{
		ResourceMesh* resourceMesh = aaa.resourceMesh;

		GLuint positionSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();

		glBufferSubData(GL_ARRAY_BUFFER, spaceUsed, positionSize, &(resourceMesh->GetVertices()[0]));

		GLuint uvOffset = positionSize + spaceUsed;
		GLuint uvSize = sizeof(float) * 2 * resourceMesh->GetNumVertices();

		float2* uvs = (float2*)(glMapBufferRange(GL_ARRAY_BUFFER, uvOffset, uvSize, GL_MAP_WRITE_BIT));

		for (unsigned int i = 0; i < resourceMesh->GetNumVertices(); ++i)
		{
			uvs[i] = float2(resourceMesh->GetTextureCoords()[i].x, resourceMesh->GetTextureCoords()[i].y);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		unsigned normalsOffset = positionSize + uvSize + spaceUsed;
		unsigned normalsSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();
		glBufferSubData(GL_ARRAY_BUFFER, normalsOffset, normalsSize, &resourceMesh->GetNormals()[0]);

		if (resourceMesh->GetTangents().size() != 0)
		{
			unsigned tangentsOffset = positionSize + uvSize + normalsSize + spaceUsed;
			unsigned tangentsSize = sizeof(float) * 3 * resourceMesh->GetNumVertices();
			glBufferSubData(GL_ARRAY_BUFFER, tangentsOffset, tangentsSize, &resourceMesh->GetTangents()[0]);
			spaceUsed += tangentsSize;
		}
		spaceUsed += positionSize + uvSize + normalsSize;
	}
}

void GeometryBatch::CalculateEBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numTotalFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (auto resourceMesh : resourceMeshes)
	{
		for (unsigned int i = 0; i < resourceMesh.resourceMesh->GetNumFaces(); ++i)
		{
			assert(resourceMesh.resourceMesh->GetFacesIndices()[i].size() == 3); // note: assume triangles = 3 indices per face
			*(indices++) = resourceMesh.resourceMesh->GetFacesIndices()[i][0];
			*(indices++) = resourceMesh.resourceMesh->GetFacesIndices()[i][1];
			*(indices++) = resourceMesh.resourceMesh->GetFacesIndices()[i][2];
		}	
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void GeometryBatch::CreateVAO()
{
	/*glBindVertexArray(vao);
	//verify which data to send in buffer

	//vertices
	glGenBuffers(1, &buffers->instance[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->instance[0]);
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



	glBindVertexArray(0);*/
}

void GeometryBatch::UpdateVAO()
{
	/*glBindVertexArray(vao);

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

	glBindVertexArray(0);*/
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		std::shared_ptr<ResourceMesh> meshShared = newComponent->GetMesh();
		if (!meshShared)
		{
			return;
		}

		ResourceMesh* mesh = meshShared.get();
		if (components.empty())
		{
			if(!mesh->GetNormals().empty())
				flags |= HAS_NORMALS;

			if (!mesh->GetTextureCoords().empty())
				flags |= HAS_TEXTURE_COORDINATES;

			if (!mesh->GetTangents().empty())
				flags |= HAS_TANGENTS;
		}

		if (IsUniqueResourceMesh(mesh))
		{
			AAA aaa = {
				mesh,
				numTotalVertices,
				numTotalFaces
			};
			resourceMeshes.push_back(aaa);
			numTotalVertices += mesh->GetNumVertices();
			numTotalFaces += mesh->GetNumFaces();
		}
		
		components.push_back(newComponent);
	}
}

void GeometryBatch::BindBatch()
{
	//command.reserve(components.size());//need to verify the size if it's matching with uniqueComponent
	/*unsigned int instanceIndex = 0;
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

			//glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	//send to gpu
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), &commands[0], GL_STATIC_DRAW);
	//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_DYNAMIC_DRAW);

	//send in the shader
	glBindBuffer(GL_ARRAY_BUFFER, indirectBuffer);*/
}

void GeometryBatch::BindBatch2(std::vector<ComponentMeshRenderer*> componentsToRender)
{
	commands.reserve(components.size());
	unsigned program = App->program->GetProgram();
	unsigned int instanceIndex = 0;

	for (auto component : componentsToRender)
	{
		if (component) //pointer not empty
		{
			AAA aaa = FindResourceMesh(component->GetMesh().get());
			ResourceMesh* resource = aaa.resourceMesh;
			if (!resource->IsLoaded())
			{
				//gen ebo vbo and vao buffers
				resource->Load();
				if (indirectBuffer == 0) {
					glGenBuffers(1, &indirectBuffer);
				}
			}
			const float4x4& model =
				static_cast<ComponentTransform*>(component->GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			GLint programInUse;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			if (program != programInUse)
			{
				glUseProgram(program);
			}

			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);

			//do a for for all the instaces existing
			Command newCommand = { 
				resource->GetNumIndexes(),	// Number of indices in the mesh
				1,							// Number of instances to render
				aaa.indexOffset,			// Index offset in the EBO
				aaa.vertexOffset,			// Vertex offset in the VBO
				instanceIndex				// Instance Index
			};

			commands.push_back(newCommand);
			instanceIndex++;

			//glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	//send to gpu
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), &commands[0], GL_STATIC_DRAW);
	//glBufferStorage(GL_DRAW_INDIRECT_BUFFER, sizeof(commands), commands, GL_DYNAMIC_DRAW);

	//send in the shader
	glBindBuffer(GL_ARRAY_BUFFER, indirectBuffer);
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

bool GeometryBatch::IsUniqueResourceMesh(const ResourceMesh* resourceMesh)
{
	for (AAA aaa : resourceMeshes)
	{
		if (aaa.resourceMesh == resourceMesh)
		{
			return false;
		}
	}

	return true;
}

AAA GeometryBatch::FindResourceMesh(ResourceMesh* mesh)
{
	for (auto aaa : resourceMeshes)
	{
		if (aaa.resourceMesh == mesh)
		{
			return aaa;
		}
	}
	abort(); //TODO check how can do this
}

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1, &indirectBuffer);

	return true;
}
