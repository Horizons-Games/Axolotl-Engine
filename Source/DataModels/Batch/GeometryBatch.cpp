#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Resources/ResourceMesh.h"

#include "DataModels/Batch/BatchFlags.h"
#include "DataModels/Program/Program.h"

#include <gl/glew.h>
#include <assert.h>

GeometryBatch::GeometryBatch()
{
	//TODO complete
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &indirectBuffer);
	glGenBuffers(1, &verticesBuffer);
	glGenBuffers(1, &textureBuffer);
	glGenBuffers(1, &normalsBuffer);
	if (flags & HAS_TANGENTS)
	{
		glGenBuffers(1, &tangentsBuffer);
	}
	glGenBuffers(1, &transforms);
	glGenVertexArrays(1, &vao);
}

GeometryBatch::~GeometryBatch()
{
	components.clear();
	resourcesInfo.clear();
	CleanUp();
}

void GeometryBatch::FillBuffers()
{
	FillEBO();

	for (auto resInfo : resourcesInfo)
	{
		ResourceMesh* resource = resInfo.resourceMesh;
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
	}
}

void GeometryBatch::FillEBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numTotalFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (auto aaa : resourcesInfo)
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
	glBindVertexArray(vao);
	//verify which data to send in buffer
	
	FillBuffers();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//vertices
	glGenBuffers(1, &verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesToRender.size() * 3 * sizeof(float), &verticesToRender[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	//texture
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, texturesToRender.size() * 2 * sizeof(float), &texturesToRender[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);

	//normals
	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalsToRender.size() * 3 * sizeof(float), &normalsToRender[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(2);

	//tangents
	if (flags & HAS_TANGENTS)
	{
		glGenBuffers(1, &tangentsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glBufferData(GL_ARRAY_BUFFER, tangentsToRender.size() * 3 * sizeof(float), &tangentsToRender[0], GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(3);
	}

	//indirect
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	glBindVertexArray(0);
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

		for (std::list<ResourceInfo>::iterator it = resourcesInfo.begin(); it != resourcesInfo.end(); ++it) {
			if (it->resourceMesh == mesh)
			{
				--it->timesRepeated;
				if (it->timesRepeated <= 0)
				{
					resourcesInfo.erase(it);
					RecalculateInfoResource();
					break;
				}
			}
		}
		components.erase(std::find(components.begin(), components.end(), component));
	}
}

void GeometryBatch::BindBatch(std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	const GLuint bindingPointCamera = 0;
	const GLuint bindingPointModel = 10;
	std::vector<float4x4>modelMatrices;
	//model(transforms)
	glGenBuffers(1, &transforms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms);
	glBufferData(GL_SHADER_STORAGE_BUFFER, componentsToRender.size() * sizeof(float4x4), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms);
	//camera
	GLuint cameraUniformBlockID;
	glGenBuffers(1, &cameraUniformBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBlockID);

	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);
	program->Activate();

	commands.clear();
	commands.reserve(componentsToRender.size());
	
	resourceMeshIndex = 0;

	for (auto component : componentsToRender)
	{
		if (component) //pointer not empty
		{
			ResourceInfo resourceInfo = FindResourceMesh(component->GetMesh().get());
			ResourceMesh* resource = resourceInfo.resourceMesh;
			
			const float4x4& model =
				static_cast<ComponentTransform*>(component->GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			modelMatrices.push_back(model);
			
			//do a for for all the instaces existing
			Command newCommand = { 
				resource->GetNumIndexes(),	// Number of indices in the mesh
				1,							// Number of instances to render
				resourceInfo.indexOffset,	// Index offset in the EBO
				resourceInfo.vertexOffset,	// Vertex offset in the VBO
				resourceMeshIndex			// Instance Index
			};

			commands.push_back(newCommand);
			resourceMeshIndex++;
		}
	}
	
	glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(Command), &commands[0], GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, modelMatrices.size()*sizeof(float4x4), modelMatrices.data());
	glBindVertexArray(vao);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, resourceMeshIndex, 0);
	glBindVertexArray(0);
	program->Deactivate();
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
	for (ResourceInfo aaa : resourcesInfo)
	{
		if (aaa.resourceMesh == resourceMesh)
		{
			++aaa.timesRepeated;
			return;
		}
	}

	ResourceInfo aaa = {
				resourceMesh,
				numTotalVertices,
				numTotalIndices,
				1
	};
	resourcesInfo.push_back(aaa);
	numTotalVertices += resourceMesh->GetNumVertices();
	numTotalIndices += resourceMesh->GetNumIndexes();
	numTotalFaces += resourceMesh->GetNumFaces();
}

void GeometryBatch::RecalculateInfoResource()
{
	numTotalVertices = 0;
	numTotalIndices = 0;
	numTotalFaces = 0;
	for (auto meshInfo : resourcesInfo)
	{
		meshInfo.indexOffset = numTotalIndices;
		meshInfo.vertexOffset = numTotalVertices;
		numTotalVertices += meshInfo.resourceMesh->GetNumVertices();
		numTotalIndices += meshInfo.resourceMesh->GetNumIndexes();
		numTotalFaces += meshInfo.resourceMesh->GetNumFaces();
	}
}

ResourceInfo& GeometryBatch::FindResourceMesh(ResourceMesh* mesh)
{
	for (auto aaa : resourcesInfo)
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
	glDeleteBuffers(1, &indirectBuffer);
	glDeleteBuffers(1, &verticesBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalsBuffer);
	if (flags & HAS_TANGENTS)
	{
		glDeleteBuffers(1, &tangentsBuffer);
	}
	glDeleteBuffers(1, &transforms);

	return true;
}
