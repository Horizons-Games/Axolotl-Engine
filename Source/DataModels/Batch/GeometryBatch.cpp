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
	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}
	//TODO complete
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	CreateVAO();
}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	resourceMeshes.clear();
	CleanUp();
}

void GeometryBatch::CalculateVBO()
{
	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

							//position			//uv				//normal	
	unsigned vertexSize = (sizeof(float) * 3 + sizeof(float) * 2 + sizeof(float) * 3);

	//tangents
	if (flags & HAS_TANGENTS)
	{
		vertexSize += sizeof(float) * 3;
	}

	GLuint bufferSize = vertexSize * numTotalVertices;

	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);

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
	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numTotalFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_DYNAMIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (auto aaa : resourceMeshes)
	{
		for (unsigned int i = 0; i < aaa.resourceMesh->GetNumFaces(); ++i)
		{
			assert(aaa.resourceMesh->GetFacesIndices()[i].size() == 3); // note: assume triangles = 3 indices per face
			*(indices++) = aaa.resourceMesh->GetFacesIndices()[i][0];
			*(indices++) = aaa.resourceMesh->GetFacesIndices()[i][1];
			*(indices++) = aaa.resourceMesh->GetFacesIndices()[i][2];
		}	
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void GeometryBatch::CreateVAO()
{
	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}
	glBindVertexArray(vao);
	//verify which data to send in buffer

	//vertices
	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3, static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	
	//texture
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	
	//normals
	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(2);
	
	if (flags & HAS_TANGENTS)
	{
		glGenBuffers(1, &tangentsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(3);
	}
	//tangents
	

	glGenBuffers(1, &indirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	glBindVertexArray(0);
}

void GeometryBatch::UpdateVAO()
{
	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}

	glBindVertexArray(vao);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesToRender.size() * sizeof(float), &verticesToRender[0], GL_DYNAMIC_DRAW);

	//texture
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, texturesToRender.size() * sizeof(float), &texturesToRender[0], GL_DYNAMIC_DRAW);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalsToRender.size() * sizeof(float), &normalsToRender[0], GL_DYNAMIC_DRAW);

	//tangents
	if (flags & HAS_TANGENTS)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glBufferData(GL_ARRAY_BUFFER, tangentsToRender.size() * sizeof(float), &tangentsToRender[0], GL_DYNAMIC_DRAW);
	}
	
	//indirect
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(Command), &commands[0], GL_DYNAMIC_DRAW);
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
			if (mesh->GetNormals().size() != 0)
			{
				flags |= HAS_NORMALS;
			}

			if (mesh->GetTextureCoords().size() != 0)
			{
				flags |= HAS_TEXTURE_COORDINATES;
			}

			if (mesh->GetTangents().size() != 0)
			{
				flags |= HAS_TANGENTS;
			}
		}
		
		CreateOrCountInstance(mesh);
		newComponent->SetBatch(this);
		components.push_back(newComponent);
	}
}

void GeometryBatch::RemoveComponent(ComponentMeshRenderer* component)
{
	if (component)
	{
		ResourceMesh* mesh = component->GetMesh().get();

		for (std::list<AAA>::iterator it = resourceMeshes.begin(); it != resourceMeshes.end(); ++it) {
			if (it->resourceMesh == mesh)
			{
				--it->timesRepeated;
				if (it->timesRepeated <= 0)
				{
					resourceMeshes.erase(it);
					RecalculateInfoResource();
					break;
				}
			}
		}
		components.erase(std::find(components.begin(), components.end(), component));
	}
}

//void GeometryBatch::BindBatch()
//{
//	const GLuint bindingPointCamera = 0;
//	const GLuint bindingPointModel = 10;
//	float4x4 modelMatrices[2];
//	int i = 0;
//	//model(transforms)
//	glGenBuffers(1, &transforms);
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms);
//	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(modelMatrices), NULL, GL_DYNAMIC_DRAW);
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms);
//	//camera
//	GLuint cameraUniformBlockID;
//	glGenBuffers(1, &cameraUniformBlockID);
//	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlockID);
//
//	for (ResourceMesh* resourceMesh : resourceMeshes)
//	{
//		if (resourceMesh) //pointer not empty
//		{
//			unsigned program = App->program->GetProgram();
//			const float4x4& view = App->engineCamera->GetCamera()->GetViewMatrix();
//			const float4x4& proj = App->engineCamera->GetCamera()->GetProjectionMatrix();
//			const float4x4& model =
//				static_cast<ComponentTransform*>(GetComponentOwner(resourceMesh)
//					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
//
//			GLint programInUse;
//			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);
//
//			if (program != programInUse)
//			{
//				glUseProgram(program);
//			}
//
//			modelMatrices[i] = model;
//			i++;
//			//binding the uniform camera
//			GLint cameraUniformBlockIndex = glGetUniformBlockIndex(program, "Camera");
//			glUniformBlockBinding(program, cameraUniformBlockIndex, bindingPointCamera);
//
//			glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
//			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), (const float*)&proj);
//			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), (const float*)&view);
//			glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointCamera, cameraUniformBlockID);
//
//		}
//	}
//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(modelMatrices), &modelMatrices);
//	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
//}

void GeometryBatch::BindBatch2(std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	const GLuint bindingPointCamera = 0;
	const GLuint bindingPointModel = 10;
	float4x4 modelMatrices[sizeof(componentsToRender)];
	//model(transforms)
	glGenBuffers(1, &transforms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(modelMatrices), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms);
	//camera
	GLuint cameraUniformBlockID;
	glGenBuffers(1, &cameraUniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlockID);

	unsigned program = App->program->GetProgram();
	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}

	commands.clear();
	commands.reserve(components.size());

	verticesToRender.clear();
	texturesToRender.clear();
	normalsToRender.clear();
	tangentsToRender.clear();
	
	resourceMeshIndex = 0;

	for (auto component : componentsToRender)
	{
		if (component) //pointer not empty
		{
			AAA aaa = FindResourceMesh(component->GetMesh().get());
			ResourceMesh* resource = aaa.resourceMesh;
			verticesToRender.insert(std::end(verticesToRender), 
				std::begin(resource->GetVertices()), std::end(resource->GetVertices()));
			
			for (float3 tex : resource->GetTextureCoords())
			{
				texturesToRender.push_back(float2(tex.x, tex.y));
			}
			
			normalsToRender.insert(std::end(normalsToRender), 
				std::begin(resource->GetNormals()), std::end(resource->GetNormals()));

			if (flags & HAS_TANGENTS)
			{
				tangentsToRender.insert(std::end(tangentsToRender), 
					std::begin(resource->GetTangents()), std::end(resource->GetTangents()));
			}
			const float4x4& view = App->engineCamera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->engineCamera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(component->GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			modelMatrices[resourceMeshIndex]=model;
			//binding the uniform camera
			GLint cameraUniformBlockIndex = glGetUniformBlockIndex(program, "Camera");
			glUniformBlockBinding(program, cameraUniformBlockIndex, bindingPointCamera);

			glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float4x4), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4x4), (const float*)&proj);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float4x4), sizeof(float4x4), (const float*)&view);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointCamera, cameraUniformBlockID);

			//do a for for all the instaces existing
			Command newCommand = { 
				resource->GetNumIndexes(),	// Number of indices in the mesh
				1,							// Number of instances to render
				aaa.indexOffset,			// Index offset in the EBO
				aaa.vertexOffset,			// Vertex offset in the VBO
				resourceMeshIndex			// Instance Index
			};

			commands.push_back(newCommand);
			resourceMeshIndex++;
		}
	}
	UpdateVAO();
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(modelMatrices), &modelMatrices);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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

void GeometryBatch::CreateOrCountInstance(ResourceMesh* resourceMesh)
{
	for (AAA aaa : resourceMeshes)
	{
		if (aaa.resourceMesh == resourceMesh)
		{
			++aaa.timesRepeated;
			return;
		}
	}

	AAA aaa = {
				resourceMesh,
				numTotalVertices,
				numTotalIndices,
				1
	};
	resourceMeshes.push_back(aaa);
	numTotalVertices += resourceMesh->GetNumVertices();
	numTotalIndices += resourceMesh->GetNumIndexes();
	numTotalFaces += resourceMesh->GetNumFaces();
}

void GeometryBatch::RecalculateInfoResource()
{
	numTotalVertices = 0;
	numTotalIndices = 0;
	numTotalFaces = 0;
	for (auto meshInfo : resourceMeshes)
	{
		meshInfo.indexOffset = numTotalIndices;
		meshInfo.vertexOffset = numTotalVertices;
		numTotalVertices += meshInfo.resourceMesh->GetNumVertices();
		numTotalIndices += meshInfo.resourceMesh->GetNumIndexes();
		numTotalFaces += meshInfo.resourceMesh->GetNumFaces();
	}
}

AAA& GeometryBatch::FindResourceMesh(ResourceMesh* mesh)
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
	glDeleteBuffers(1, &verticesBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalsBuffer);
	glDeleteBuffers(1, &tangentsBuffer);

	return true;
}
