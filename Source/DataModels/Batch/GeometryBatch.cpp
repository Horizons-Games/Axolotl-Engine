#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include "DataModels/Program/Program.h"

#include "GameObject/GameObject.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"

#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"

#ifndef ENGINE
#include "FileSystem/ModuleResources.h"
#endif // !ENGINE

GeometryBatch::GeometryBatch(int flags) : numTotalVertices(0), numTotalIndices(0), numTotalFaces(0), 
	createBuffers(true), reserveModelSpace(true), flags(flags), fillMaterials(false), frame(0),
	defaultMaterial(new ResourceMaterial(0, "", "", "")),
	mapFlags(GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT),
	createFlags(mapFlags | GL_DYNAMIC_STORAGE_BIT)
{
	if (this->flags & BatchManager::HAS_SPECULAR)
	{
		program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SPECULAR);
	}
	else 
	{
		program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::DEFAULT);
	}

	//initialize buffers
	CreateVAO();
}

GeometryBatch::~GeometryBatch()
{
	for (ResourceInfo* resourceInfo : resourcesInfo)
	{
		delete resourceInfo;
	}
	resourcesInfo.clear();

	for (ComponentMeshRenderer* component : componentsInBatch)
	{
		if (component)
		{
			component->SetBatch(nullptr);
		}
	}
	componentsInBatch.clear();

	objectIndexes.clear();
	instanceData.clear();
	perInstances.clear();
	paletteIndexes.clear();

	CleanUp();
	
	for (int i = 0; i < DOUBLE_BUFFERS; i++)
	{
		if (gSync[i])
		{
			glDeleteSync(gSync[i]);
		}
	}
}

void GeometryBatch::FillBuffers()
{
	FillEBO();

	std::vector<float3> verticesToRender;
	std::vector<float2> texturesToRender;
	std::vector<float3> normalsToRender;
	std::vector<float3> tangentsToRender;
	std::vector<uint4> bonesToRender;
	std::vector<float4> weightsToRender;
	
	verticesToRender.reserve(numTotalVertices);
	texturesToRender.reserve(numTotalVertices);
	normalsToRender.reserve(numTotalVertices);
	bonesToRender.reserve(numTotalVertices);
	weightsToRender.reserve(numTotalVertices);

	if (flags & BatchManager::HAS_TANGENTS)
	{
		tangentsToRender.reserve(numTotalVertices);
	}

	for (auto resInfo : resourcesInfo)
	{
		std::shared_ptr<ResourceMesh> resource = resInfo->resourceMesh;
		verticesToRender.insert(std::end(verticesToRender),
			std::begin(resource->GetVertices()), std::end(resource->GetVertices()));
		
		
		for (float3 tex : resource->GetTextureCoords())
		{
			texturesToRender.push_back(float2(tex.x, tex.y));
		}

		normalsToRender.insert(std::end(normalsToRender),
			std::begin(resource->GetNormals()), std::end(resource->GetNormals()));

		if (flags & BatchManager::HAS_TANGENTS)
		{
			tangentsToRender.insert(std::end(tangentsToRender),
				std::begin(resource->GetTangents()), std::end(resource->GetTangents()));
		}

		for (unsigned int i = 0; i < resource->GetNumVertices(); ++i)
		{
			Attach attach = resource->GetAttaches()[i];

			bonesToRender.push_back(uint4(attach.bones[0], attach.bones[1],
										  attach.bones[2], attach.bones[3]));

			weightsToRender.push_back(float4(attach.weights[0], attach.weights[1],
											 attach.weights[2], attach.weights[3]));
		}

	}

	glNamedBufferData(verticesBuffer, verticesToRender.size() * 3 * sizeof(float), &verticesToRender[0], GL_STATIC_DRAW);
	glNamedBufferData(textureBuffer, texturesToRender.size() * 2 * sizeof(float), &texturesToRender[0], GL_STATIC_DRAW);
	glNamedBufferData(normalsBuffer, normalsToRender.size() * 3 * sizeof(float), &normalsToRender[0], GL_STATIC_DRAW);

	if (flags & BatchManager::HAS_TANGENTS)
	{
		glNamedBufferData(tangentsBuffer, tangentsToRender.size() * 3 * sizeof(float), &tangentsToRender[0], GL_STATIC_DRAW);
	}
	
	glNamedBufferData(bonesBuffer, bonesToRender.size() * sizeof(uint4), &bonesToRender[0], GL_STATIC_DRAW);
	glNamedBufferData(weightsBuffer, weightsToRender.size() * sizeof(float4), &weightsToRender[0], GL_STATIC_DRAW);
}

void GeometryBatch::FillMaterial()
{
	fillMaterials = false;

	for (int i = 0; i < instanceData.size(); i++)
	{
		int materialIndex = instanceData[i];
		std::shared_ptr<ResourceMaterial> resourceMaterial = resourcesMaterial[materialIndex];

		if (flags & BatchManager::HAS_METALLIC)
		{
			MaterialMetallic newMaterial =
			{
				resourceMaterial->GetDiffuseColor(),
				resourceMaterial->HasDiffuse(),
				resourceMaterial->HasNormal(),
				resourceMaterial->HasMetallic(),
				resourceMaterial->GetSmoothness(),
				resourceMaterial->GetMetalness(),
				resourceMaterial->GetNormalStrength()
			};

			std::shared_ptr<ResourceTexture> texture = resourceMaterial->GetDiffuse();
			if (texture)
			{
				newMaterial.diffuse_map = texture->GetHandle();
			}

			texture = resourceMaterial->GetNormal();
			if (texture)
			{
				newMaterial.normal_map = texture->GetHandle();
			}

			texture = resourceMaterial->GetMetallic();
			if (texture)
			{
				newMaterial.metallic_map = texture->GetHandle();
			}
			metallicMaterialData[i] = newMaterial;
		}

		else if (flags & BatchManager::HAS_SPECULAR)
		{
			MaterialSpecular newMaterial =
			{
				resourceMaterial->GetDiffuseColor(),
				resourceMaterial->GetSpecularColor(),
				resourceMaterial->HasDiffuse(),
				resourceMaterial->HasNormal(),
				resourceMaterial->HasSpecular(),
				resourceMaterial->GetSmoothness(),
				resourceMaterial->GetMetalness(),
				resourceMaterial->GetNormalStrength()
			};

			std::shared_ptr<ResourceTexture> texture = resourceMaterial->GetDiffuse();
			if (texture)
			{
				newMaterial.diffuse_map = texture->GetHandle();
			}

			texture = resourceMaterial->GetNormal();
			if (texture)
			{
				newMaterial.normal_map = texture->GetHandle();
			}

			texture = resourceMaterial->GetSpecular();
			if (texture)
			{
				newMaterial.specular_map = texture->GetHandle();
			}
			specularMaterialData[i] = newMaterial;
		}
	}
}

void GeometryBatch::FillEBO()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint indexSize = sizeof(GLuint) * numTotalFaces * 3;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (auto info : resourcesInfo)
	{
		for (unsigned int i = 0; i < info->resourceMesh->GetNumFaces(); i++)
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
	if (vao == 0)
	{
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);

	//verify which data to send in buffer
	if (ebo == 0)
	{
		glGenBuffers(1, &ebo);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//vertices
	glEnableVertexAttribArray(0);
	if (verticesBuffer == 0)
	{
		glGenBuffers(1, &verticesBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

	//texture
	glEnableVertexAttribArray(1);
	if (textureBuffer == 0)
	{
		glGenBuffers(1, &textureBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));

	//normals
	glEnableVertexAttribArray(2);
	if (normalsBuffer == 0)
	{
		glGenBuffers(1, &normalsBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

	//tangents
	glEnableVertexAttribArray(3);
	if (tangentsBuffer == 0)
	{
		glGenBuffers(1, &tangentsBuffer);
	}
	if (flags & BatchManager::HAS_TANGENTS)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	}

	//bones
	glEnableVertexAttribArray(4);
	if (bonesBuffer == 0)
	{
		glGenBuffers(1, &bonesBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, bonesBuffer);
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, sizeof(uint4), static_cast<void*>(nullptr));

	//weights
	glEnableVertexAttribArray(5);
	if (weightsBuffer == 0)
	{
		glGenBuffers(1, &weightsBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, weightsBuffer);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));

	//indirect
	if (indirectBuffer == 0)
	{
		glGenBuffers(1, &indirectBuffer);
	}
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	//glBufferData(GL_DRAW_INDIRECT_BUFFER, 0 * sizeof(Command), nullptr, GL_DYNAMIC_DRAW);

	for (int i = 0; i < DOUBLE_BUFFERS; i++)
	{
		if (transforms[i] == 0)
		{
			glGenBuffers(1, &transforms[i]);
		}
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms[i], 0, 
			componentsInBatch.size() * sizeof(float4x4));
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(float4x4), nullptr, createFlags);

		transformData[i] = static_cast<float4x4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
			componentsInBatch.size() * sizeof(float4x4), mapFlags));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Materials
	if (materials == 0)
	{
		glGenBuffers(1, &materials);
	}

	if (flags & BatchManager::HAS_METALLIC)
	{
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointMaterial, materials, 0, componentsInBatch.size() * 
			sizeof(MaterialMetallic));

		glBufferStorage(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(MaterialMetallic), nullptr, 
			createFlags);

		metallicMaterialData = static_cast<MaterialMetallic*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
			componentsInBatch.size() * sizeof(MaterialMetallic), mapFlags));
	}

	else if (flags & BatchManager::HAS_SPECULAR)
	{
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointMaterial, materials, 0, componentsInBatch.size() * 
			sizeof(MaterialSpecular));

		glBufferStorage(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(MaterialSpecular), nullptr, 
			createFlags);

		specularMaterialData = static_cast<MaterialSpecular*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
			componentsInBatch.size() * sizeof(MaterialSpecular), mapFlags));
	}

	// Palettes & PerInstances
	unsigned int totalNumBones = 0;
	unsigned count = 0;

	for (const ComponentMeshRenderer* component : componentsInBatch)
	{
		PerInstance instance;

		int numBones = component->GetMesh()->GetNumBones();

		instance.numBones = numBones;
		instance.paletteOffset = totalNumBones;

		perInstances.push_back(instance);

		paletteIndexes[component] = count;

		totalNumBones += numBones;
		++count;
	}

	// Palettes
	for (int i = 0; i < DOUBLE_BUFFERS; i++)
	{
		if (palettes[i] == 0)
		{
			glGenBuffers(1, &palettes[i]);
		}
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointPalette, palettes[i], 0, 
						  totalNumBones * sizeof(float4x4));
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, totalNumBones * sizeof(float4x4), nullptr, createFlags);

		paletteData[i] = static_cast<float4x4*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 
											    totalNumBones * sizeof(float4x4), mapFlags));
	}

	// PerInstances
	if (perInstancesBuffer == 0)
	{
		glGenBuffers(1, &perInstancesBuffer);
	}
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointPerInstance, perInstancesBuffer, 0, 
					  perInstances.size() * sizeof(PerInstance));
	if (perInstances.size() > 0)
	{
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, perInstances.size() * sizeof(PerInstance), &perInstances[0], 
					    createFlags);
	}
	else
	{
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, perInstances.size() * sizeof(PerInstance), nullptr, createFlags);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindVertexArray(0);
}

void GeometryBatch::ClearBuffer()
{
	glDeleteBuffers(1, &indirectBuffer);
	glDeleteBuffers(1, &materials);
	glDeleteBuffers(1, &perInstancesBuffer);
	glDeleteBuffers(DOUBLE_BUFFERS, &transforms[0]);
	glDeleteBuffers(DOUBLE_BUFFERS, &palettes[0]);
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
		
		CreateInstanceResourceMesh(meshShared);
		objectIndexes[newComponent] = CreateInstanceResourceMaterial(materialShared);
		newComponent->SetBatch(this);
		componentsInBatch.push_back(newComponent);

		perInstances.clear();
		paletteIndexes.clear();

		fillMaterials = true;
		reserveModelSpace = true;
		dirtyBatch = true;
	}
}

void GeometryBatch::DeleteComponent(ComponentMeshRenderer* componentToDelete)
{
	bool findMesh = false;
	bool findMaterial = false;

	for (const ComponentMeshRenderer* compare : componentsInBatch)
	{
		if (compare->GetMesh().get() == componentToDelete->GetMesh().get() && compare != componentToDelete)
		{
			findMesh = true;
		}
		if (compare->GetMaterial().get() == componentToDelete->GetMaterial().get() && compare != componentToDelete)
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
		for (auto it = resourcesInfo.begin(); it != resourcesInfo.end(); it++) {
			if ((*it)->resourceMesh == componentToDelete->GetMesh())
			{
				delete (*it);
				resourcesInfo.erase(it);
				break;
			}
		}
		createBuffers = true;
#else
		App->GetModule<ModuleResources>()->FillResourceBin(componentToDelete->GetMesh());
#endif //ENGINE
	}
	if (!findMaterial)
	{
#ifdef ENGINE
		auto it = std::find(resourcesMaterial.begin(), resourcesMaterial.end(), componentToDelete->GetMaterial());
		// If element was found
		if (it != resourcesMaterial.end())
		{
			resourcesMaterial.erase(it);
		}
	}

	componentsInBatch.erase(std::find(componentsInBatch.begin(), componentsInBatch.end(), componentToDelete));
	resourcesInfo.clear();
	resourcesMaterial.clear();
	instanceData.clear();
	perInstances.clear();
	paletteIndexes.clear();

	fillMaterials = true;
	reserveModelSpace = true;
	dirtyBatch = true;
	componentToDelete->SetBatch(nullptr);

#else
		App->GetModule<ModuleResources>()->FillResourceBin(componentToDelete->GetMaterial());
	}
#endif //ENGINE
}

std::vector<ComponentMeshRenderer*> GeometryBatch::ChangeBatch(const ComponentMeshRenderer* componentToDelete)
{
	std::vector<ComponentMeshRenderer*> componentToMove;

	for (ComponentMeshRenderer* compare : componentsInBatch)
	{
		bool findMaterial = false;

		if (compare->GetMaterial().get() == componentToDelete->GetMaterial().get())
		{
			findMaterial = true;
		}

		if (findMaterial)
		{
			componentToMove.push_back(compare);
			for (auto it = resourcesInfo.begin(); it != resourcesInfo.end(); it++)
			{
				if ((*it)->resourceMesh == compare->GetMesh())
				{
					delete (*it);
					resourcesInfo.erase(it);
					break;
				}
			}
			createBuffers = true;

			auto it = std::find(resourcesMaterial.begin(), resourcesMaterial.end(), compare->GetMaterial());
			// If element was found
			if (it != resourcesMaterial.end())
			{
				resourcesMaterial.erase(it);
			}
		}
	}

	for (ComponentMeshRenderer* compare : componentToMove)
	{
		componentsInBatch.erase(std::find(componentsInBatch.begin(), componentsInBatch.end(), compare));
	}
	resourcesInfo.clear();
	resourcesMaterial.clear();
	instanceData.clear();
	perInstances.clear();
	paletteIndexes.clear();

	reserveModelSpace = true;
	fillMaterials = true;
	dirtyBatch = true;

	return componentToMove;
}


void GeometryBatch::DeleteMaterial(const ComponentMeshRenderer* componentToDelete)
{
	resourcesMaterial.erase(
			std::find(resourcesMaterial.begin(), resourcesMaterial.end(), componentToDelete->GetMaterial()));
	
	perInstances.clear();
	paletteIndexes.clear();

	reserveModelSpace = true;
	fillMaterials = true;
	dirtyBatch = true;
}

void GeometryBatch::BindBatch(bool selected)
{
	program->Activate();

	frame = (frame + 1) % DOUBLE_BUFFERS;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms[frame]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointPalette, palettes[frame]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointPerInstance, perInstancesBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointMaterial, materials);

	WaitBuffer();
	
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

	if (reserveModelSpace)
	{
		//Redo instanceData
		instanceData.clear();
		instanceData.reserve(componentsInBatch.size());
		for (const ComponentMeshRenderer* component : componentsInBatch)
		{
			if (component->GetMaterial())
			{
				objectIndexes[component] = CreateInstanceResourceMaterial(component->GetMaterial());
			}
			else
			{
				objectIndexes[component] = CreateInstanceResourceMaterial(defaultMaterial);
			}
		}
		reserveModelSpace = false;
	}

	if (fillMaterials)
	{
		FillMaterial();
	}

	std::vector<Command> commands;
	commands.reserve(componentsInBatch.size());
	
	int drawCount = 0;

	GameObject* selectedGo = App->GetModule<ModuleScene>()->GetSelectedGameObject();
	bool isRoot = selectedGo->GetParent() == nullptr;

	for (auto component : componentsInBatch)
	{
		assert(component);

		const GameObject* owner = component->GetOwner();

		if (App->GetModule<ModuleRender>()->IsObjectInsideFrustrum(owner))
		{
#ifdef ENGINE
			bool draw = false;

			if (!App->IsOnPlayMode() && !isRoot)
			{
				if (!selected)
				{
					if (owner != selectedGo && !selectedGo->IsADescendant(owner))
					{
						draw = true;
					}
				}
				else
				{
					if (owner == selectedGo || selectedGo->IsADescendant(owner))
					{
						draw = true;
					}
				}
			}
			else
			{
				draw = true;
			}

			if (draw)
			{
				//component->InitBones();
				component->UpdatePalette();

				ResourceInfo* resourceInfo = FindResourceInfo(component->GetMesh());
				std::shared_ptr<ResourceMesh> resource = resourceInfo->resourceMesh;

				// find position in components vector
				unsigned int instanceIndex = objectIndexes[component];
				unsigned int paletteIndex = paletteIndexes[component];

				transformData[frame][instanceIndex] = static_cast<ComponentTransform*>(
					component->GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
				
				if (component->GetMesh()->GetNumBones() > 0)
				{
					memcpy(&paletteData[frame][perInstances[paletteIndex].paletteOffset],
						&component->GetPalette()[0],
						perInstances[paletteIndex].numBones * sizeof(float4x4));
				}

				//do a for for all the instaces existing
				Command newCommand {
					resource->GetNumIndexes(),	// Number of indices in the mesh
					1,							// Number of instances to render
					resourceInfo->indexOffset,	// Index offset in the EBO
					resourceInfo->vertexOffset, // Vertex offset in the VBO
					instanceIndex				// Instance Index
				};

				commands.push_back(newCommand);
				drawCount++;
			}
#else
			component->InitBones();
			component->UpdatePalette();

			ResourceInfo* resourceInfo = FindResourceInfo(component->GetMesh());
			std::shared_ptr<ResourceMesh> resource = resourceInfo->resourceMesh;

			// find position in components vector
			unsigned int instanceIndex = objectIndexes[component];
			unsigned int paletteIndex = paletteIndexes[component];

			transformData[frame][instanceIndex] = static_cast<ComponentTransform*>(
				component->GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			if (component->GetMesh()->GetNumBones() > 0)
			{
				std::vector<float4x4> palettes = component->GetPalette();

				memcpy(&paletteData[frame][perInstances[paletteIndex].paletteOffset],
					&component->GetPalette()[0],
					perInstances[paletteIndex].numBones * sizeof(float4x4));
			}

			//do a for for all the instaces existing
			Command newCommand{
				resource->GetNumIndexes(),	// Number of indices in the mesh
				1,							// Number of instances to render
				resourceInfo->indexOffset,	// Index offset in the EBO
				resourceInfo->vertexOffset, // Vertex offset in the VBO
				instanceIndex				// Instance Index
			};

			commands.push_back(newCommand);
			drawCount++;
#endif //ENGINE
		}
	}
	
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
	if (commands.size() > 0)
	{
		glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(Command), &commands[0], GL_DYNAMIC_DRAW);
	}
	else
	{
		glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(Command), nullptr, GL_DYNAMIC_DRAW);
	}
	glBindVertexArray(vao);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, drawCount, 0);
	LockBuffer();
	
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindVertexArray(0);

	program->Deactivate();
}

void GeometryBatch::CreateInstanceResourceMesh(std::shared_ptr<ResourceMesh> mesh)
{
	for (const ResourceInfo* info : resourcesInfo)
	{
		if (info->resourceMesh == mesh)
		{
			return;
		}
	}

	ResourceInfo* resourceInfo = new ResourceInfo;
	resourceInfo->resourceMesh = mesh;
	resourcesInfo.push_back(resourceInfo);
	createBuffers = true;
}

int GeometryBatch::CreateInstanceResourceMaterial(const std::shared_ptr<ResourceMaterial> material)
{
	auto it = std::find(resourcesMaterial.begin(), resourcesMaterial.end(), material);
	int index;
	// If element was found
	if (it != resourcesMaterial.end())
	{
		// calculating the index
		index = static_cast<int>(it - resourcesMaterial.begin());
		instanceData.push_back(index);
	}
	else {
		// If the element is not
		// present in the vector
		resourcesMaterial.push_back(material);
		index = static_cast<int>(resourcesMaterial.size()) - 1;
		instanceData.push_back(index);
	}
	
	return instanceData.size() - 1;
}

GeometryBatch::ResourceInfo* GeometryBatch::FindResourceInfo(const std::shared_ptr<ResourceMesh> mesh)
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

void GeometryBatch::CleanUp()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &indirectBuffer);
	glDeleteBuffers(1, &verticesBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalsBuffer);
	glDeleteBuffers(1, &tangentsBuffer);
	glDeleteBuffers(1, &weightsBuffer);
	glDeleteBuffers(1, &bonesBuffer);
	glDeleteBuffers(1, &materials);
	glDeleteBuffers(1, &perInstancesBuffer);
	glDeleteBuffers(DOUBLE_BUFFERS, &transforms[0]);
	glDeleteBuffers(DOUBLE_BUFFERS, &palettes[0]);
}

void GeometryBatch::UpdateBatchComponents()
{
	for (const ComponentMeshRenderer* component : componentsInBatch)
	{
		std::shared_ptr<ResourceMesh> meshShared = component->GetMesh();
		std::shared_ptr<ResourceMaterial> materialShared = component->GetMaterial();
		if (!meshShared)
		{
			return;
		}

		CreateInstanceResourceMesh(meshShared);
		reserveModelSpace = true;
	}
}

bool CompareFarToClose(const ComponentMeshRenderer* first, const ComponentMeshRenderer* second)
{
	float firstDistance = App->GetModule<ModuleRender>()->GetObjectDistance(first->GetOwner());
	float secondDistance = App->GetModule<ModuleRender>()->GetObjectDistance(second->GetOwner());

	return (firstDistance > secondDistance);
}

bool CompareCloseToFar(const ComponentMeshRenderer* first, const ComponentMeshRenderer* second)
{
	float firstDistance = App->GetModule<ModuleRender>()->GetObjectDistance(first->GetOwner());
	float secondDistance = App->GetModule<ModuleRender>()->GetObjectDistance(second->GetOwner());

	return (firstDistance < secondDistance);
}

void GeometryBatch::SortByDistanceFarToClose()
{
	std::sort(componentsInBatch.begin(), componentsInBatch.end(), CompareFarToClose);
}

void GeometryBatch::SortByDistanceCloseToFar()
{
	std::sort(componentsInBatch.begin(), componentsInBatch.end(), CompareCloseToFar);
}


void GeometryBatch::SetDirty(const bool dirty)
{
	dirtyBatch = dirty;
}

void GeometryBatch::LockBuffer()
{
	if (gSync[frame])
	{
		glDeleteSync(gSync[frame]);
	}
	gSync[frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void GeometryBatch::WaitBuffer()
{
	if (gSync[frame])
	{
		while (1)
		{
			GLenum waitReturn = glClientWaitSync(gSync[frame], GL_SYNC_FLUSH_COMMANDS_BIT, 1000);
			if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
			{
				return;
			}
		}
	}
}
