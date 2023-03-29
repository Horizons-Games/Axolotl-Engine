#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"

#include "DataModels/Batch/BatchFlags.h"
#include "DataModels/Program/Program.h"

#ifndef ENGINE
#include "FileSystem/ModuleResources.h"
#endif // !ENGINE

GeometryBatch::GeometryBatch()
{
	//initialize buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &indirectBuffer);
	glGenBuffers(1, &transforms);
	glGenBuffers(1, &verticesBuffer);
	glGenBuffers(1, &textureBuffer);
	glGenBuffers(1, &normalsBuffer);
	glGenBuffers(1, &tangentsBuffer);
	glGenBuffers(1, &materials);
}

GeometryBatch::~GeometryBatch()
{
	componentsInBatch.clear();
	resourcesInfo.clear();
	resourcesMaterial.clear();
	instanceData.clear();
	CleanUp();
}

void GeometryBatch::FillBuffers()
{
	FillEBO();

	std::vector<float3> verticesToRender;
	std::vector<float2> texturesToRender;
	std::vector<float3> normalsToRender;
	std::vector<float3> tangentsToRender;
	
	verticesToRender.reserve(numTotalVertices);
	texturesToRender.reserve(numTotalVertices);
	normalsToRender.reserve(numTotalVertices);
	if (flags & HAS_TANGENTS)
	{
		tangentsToRender.reserve(numTotalVertices);
	}

	for (auto resInfo : resourcesInfo)
	{

		ResourceMesh* resource = resInfo->resourceMesh;
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

	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticesToRender.size() * 3 * sizeof(float), &verticesToRender[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, texturesToRender.size() * 2 * sizeof(float), &texturesToRender[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, normalsToRender.size() * 3 * sizeof(float), &normalsToRender[0], GL_STATIC_DRAW);

	if (flags & HAS_TANGENTS)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glBufferData(GL_ARRAY_BUFFER, tangentsToRender.size() * 3 * sizeof(float), &tangentsToRender[0], GL_STATIC_DRAW);
	}
}

void GeometryBatch::FillMaterial()
{
	std::vector<Material> materialToRender;
	materialToRender.reserve(instanceData.size());
	for (int i = 0; i < instanceData.size(); i++)
	{
		int materialIndex = instanceData[i];
		ResourceMaterial* resourceMaterial = resourcesMaterial[materialIndex];
		Material newMaterial =
		{
		resourceMaterial->GetDiffuseColor(),
		resourceMaterial->GetNormalStrength(),
		resourceMaterial->HasDiffuse(),
		resourceMaterial->HasNormal(),
		resourceMaterial->GetSmoothness(),
		resourceMaterial->HasMetallicAlpha(),
		resourceMaterial->GetMetalness(),
		resourceMaterial->HasMetallicMap(),
		};

		std::shared_ptr<ResourceTexture> texture = resourceMaterial->GetDiffuse();
		if (texture)
		{
			newMaterial.diffuse_map = texture->GetHandle();
			glMakeTextureHandleResidentARB(newMaterial.diffuse_map);
		}

		texture = resourceMaterial->GetNormal();
		if (texture)
		{
			newMaterial.normal_map = texture->GetHandle();
			glMakeTextureHandleResidentARB(newMaterial.normal_map);
		}

		texture = resourceMaterial->GetMetallicMap();
		if (texture)
		{
			newMaterial.metallic_map = texture->GetHandle();
			glMakeTextureHandleResidentARB(newMaterial.metallic_map);
		}

		materialToRender.push_back(newMaterial);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materialToRender.size() * sizeof(Material), &materialToRender[0], GL_STATIC_DRAW);
}

void GeometryBatch::FillEBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numTotalFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (auto info : resourcesInfo)
	{
		for (unsigned int i = 0; i < info->resourceMesh->GetNumFaces(); ++i)
		{
			assert(info->resourceMesh->GetFacesIndices()[i].size() == 3); // note: assume triangles = 3 indices per face
			*(indices++) = info->resourceMesh->GetFacesIndices()[i][0];
			*(indices++) = info->resourceMesh->GetFacesIndices()[i][1];
			*(indices++) = info->resourceMesh->GetFacesIndices()[i][2];
		}	
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void GeometryBatch::CreateVAO()
{
	glBindVertexArray(vao);
	//verify which data to send in buffer
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	//texture
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);

	//normals
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(2);

	//tangents
	if (flags & HAS_TANGENTS)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(3);
	}

	//indirect
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	
	const GLuint bindingPointModel = 10;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms);	
	const GLuint bindingPointMaterial = 11;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointMaterial, materials);
	
	glBindVertexArray(0);
}

void GeometryBatch::AddComponentMeshRenderer(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		std::shared_ptr<ResourceMesh> meshShared = newComponent->GetMesh();
		std::shared_ptr<ResourceMaterial> materialShared = newComponent->GetMaterial();
		if (!meshShared)
		{
			return;
		}
		
		CreateInstanceResourceMesh(meshShared.get());
		newComponent->SetBatch(this);
		componentsInBatch.push_back(newComponent);
		reserveModelSpace = true;
	}
}

void GeometryBatch::DeleteComponent(ComponentMeshRenderer* componentToDelete)
{
	bool findMesh = false;
	bool findMaterial = false;
	for (ComponentMeshRenderer* compare : componentsInBatch)
	{
		if (compare->GetMesh() == componentToDelete->GetMesh() && compare != componentToDelete)
		{
			findMesh = true;
		}
		if (compare->GetMaterial() == componentToDelete->GetMaterial() && compare != componentToDelete)
		{
			findMaterial = true;
		}

		if (findMaterial && findMesh)
		{
			break;
		}
	}

	if (!findMesh)
	{
#ifdef ENGINE
		for (auto it = resourcesInfo.begin(); it != resourcesInfo.end(); ++it) {
			if ((*it)->resourceMesh == componentToDelete->GetMesh().get())
			{
				resourcesInfo.erase(it);
				break;
			}
		}
		createBuffers = true;
#else
		App->resources->FillResourceBin(componentToDelete->GetMesh());
#endif //ENGINE
	}
	if (!findMaterial)
	{
#ifdef ENGINE
		resourcesMaterial.erase(
			std::find(resourcesMaterial.begin(), resourcesMaterial.end(), componentToDelete->GetMaterial().get()));
	}
	componentsInBatch.erase(std::find(componentsInBatch.begin(), componentsInBatch.end(), componentToDelete));
	reserveModelSpace = true;
#else
		App->resources->FillResourceBin(componentToDelete->GetMaterial());
	}
#endif //ENGINE
}

void GeometryBatch::BindBatch(const std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	if (createBuffers)
	{
		//Redo info
		numTotalVertices = 0;
		numTotalIndices = 0;
		numTotalFaces = 0;
		for (auto info : resourcesInfo) {
			info->vertexOffset = numTotalVertices;
			info->indexOffset = numTotalIndices;
			numTotalVertices += info->resourceMesh->GetNumVertices();
			numTotalIndices += info->resourceMesh->GetNumIndexes();
			numTotalFaces += info->resourceMesh->GetNumFaces();
		}
		FillBuffers();
		createBuffers = false;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms);
	if (reserveModelSpace)
	{
		//Redo instanceData
		instanceData.clear();
		instanceData.reserve(componentsInBatch.size());
		for (ComponentMeshRenderer* component : componentsInBatch)
		{
			CreateInstanceResourceMaterial(component->GetMaterial().get());
		}
		glBufferData(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(float4x4), NULL, GL_DYNAMIC_DRAW);
		FillMaterial();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms);
		reserveModelSpace = false;
	}

	std::vector<Command> commands;
	commands.reserve(componentsToRender.size());
	
	int drawCount = 0;

	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);
	program->Activate();

	std::vector<float4x4> modelMatrices (componentsInBatch.size());

	for (auto component : componentsToRender)
	{
		assert(component);
		ResourceInfo* resourceInfo = FindResourceInfo(component->GetMesh().get());
		ResourceMesh* resource = resourceInfo->resourceMesh;
		//find position in components vector
		auto it = std::find(componentsInBatch.begin(), componentsInBatch.end(), component);

		int instanceIndex = it - componentsInBatch.begin();

		modelMatrices[instanceIndex] = static_cast<ComponentTransform*>(component->GetOwner()
			->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
			
		//do a for for all the instaces existing
		Command newCommand = { 
			resource->GetNumIndexes(),	// Number of indices in the mesh
			1,							// Number of instances to render
			resourceInfo->indexOffset,	// Index offset in the EBO
			resourceInfo->vertexOffset,	// Vertex offset in the VBO
			instanceIndex				// Instance Index
		};
		commands.push_back(newCommand);
		drawCount++;
	}
	
	glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(Command), &commands[0], GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, modelMatrices.size() * sizeof(float4x4), modelMatrices.data());
	glBindVertexArray(vao);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, drawCount, 0);
	glBindVertexArray(0);
	program->Deactivate();
}

void GeometryBatch::CreateInstanceResourceMesh(ResourceMesh* mesh)
{
	for (ResourceInfo* info : resourcesInfo)
	{
		if (info->resourceMesh == mesh)
		{
			return;
		}
	}

	if (flags == 0)
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

	ResourceInfo* resourceInfo = new ResourceInfo;
	resourceInfo->resourceMesh = mesh;
	resourcesInfo.push_back(resourceInfo);
	createBuffers = true;
}

void GeometryBatch::CreateInstanceResourceMaterial(ResourceMaterial* material)
{
	auto it = std::find(resourcesMaterial.begin(), resourcesMaterial.end(), material);

	// If element was found
	if (it != resourcesMaterial.end())
	{
		// calculating the index
		int index = it - resourcesMaterial.begin();
		instanceData.push_back(index);
	}
	else {
		// If the element is not
		// present in the vector
		resourcesMaterial.push_back(material);
		instanceData.push_back(resourcesMaterial.size() - 1);
	}
}

ResourceInfo* GeometryBatch::FindResourceInfo(ResourceMesh* mesh)
{
	for (auto info : resourcesInfo)
	{
		if (info->resourceMesh == mesh)
		{
			return info;
		}
	}
	assert(false);
	return nullptr;
}

bool GeometryBatch::CleanUp()
{
	glDeleteBuffers(1,&vao);
	glDeleteBuffers(1,&ebo);
	glDeleteBuffers(1, &indirectBuffer);
	glDeleteBuffers(1, &verticesBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalsBuffer);
	glDeleteBuffers(1, &tangentsBuffer);
	glDeleteBuffers(1, &transforms);
	glDeleteBuffers(1, &materials);

	return true;
}
