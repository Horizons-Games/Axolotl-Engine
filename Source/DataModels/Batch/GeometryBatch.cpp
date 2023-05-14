#include "GeometryBatch.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include "DataModels/Batch/BatchFlags.h"
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
createBuffers(true), reserveModelSpace(true), flags(flags), fillMaterials(false), 
defaultMaterial(new ResourceMaterial(0, "", "", "")), 
mapFlags(GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT),
createFlags(mapFlags | GL_DYNAMIC_STORAGE_BIT)
{
	// Setting the default framgent as the METALLIC (in case the mesh didn't had a material)
	this->flags |= (flags & HAS_SPECULAR || flags & HAS_METALLIC) ? 0 : HAS_METALLIC;

	if (this->flags & HAS_SPECULAR)
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
		component->SetBatch(nullptr);
	}
	componentsInBatch.clear();

	objectIndexes.clear();
	instanceData.clear();

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

	glNamedBufferData(verticesBuffer, verticesToRender.size() * 3 * sizeof(float), &verticesToRender[0], GL_STATIC_DRAW);

	glNamedBufferData(textureBuffer, texturesToRender.size() * 2 * sizeof(float), &texturesToRender[0], GL_STATIC_DRAW);

	glNamedBufferData(normalsBuffer, normalsToRender.size() * 3 * sizeof(float), &normalsToRender[0], GL_STATIC_DRAW);

	if (flags & HAS_TANGENTS)
	{
		glNamedBufferData(tangentsBuffer, tangentsToRender.size() * 3 * sizeof(float), &tangentsToRender[0], GL_STATIC_DRAW);
	}
}

void GeometryBatch::FillMaterial()
{
	fillMaterials = false;

	for (int i = 0; i < instanceData.size(); i++)
	{
		int materialIndex = instanceData[i];
		std::shared_ptr<ResourceMaterial> resourceMaterial = resourcesMaterial[materialIndex];

		if (flags & HAS_METALLIC)
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

		else if (flags & HAS_SPECULAR)
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
	if (!glIsBuffer(vao))
	{
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);

	//verify which data to send in buffer
	if (!glIsBuffer(ebo))
	{
		glGenBuffers(1, &ebo);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//vertices
	if (!glIsBuffer(verticesBuffer))
	{
		glGenBuffers(1, &verticesBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	//texture
	if (!glIsBuffer(textureBuffer))
	{
		glGenBuffers(1, &textureBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);

	//normals
	if (!glIsBuffer(normalsBuffer))
	{
		glGenBuffers(1, &normalsBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(2);

	//tangents
	if (!glIsBuffer(tangentsBuffer))
	{
		glGenBuffers(1, &tangentsBuffer);
	}
	if (flags & HAS_TANGENTS)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tangentsBuffer);
	}

	//indirect
	if (!glIsBuffer(indirectBuffer))
	{
		glGenBuffers(1, &indirectBuffer);
	}
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);

	for (int i = 0; i < DOUBLE_BUFFERS; i++)
	{
		if (!glIsBuffer(transforms[i]))
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

	if (!glIsBuffer(materials))
	{
		glGenBuffers(1, &materials);
	}
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPointMaterial, materials, 0, 
		componentsInBatch.size() * sizeof(float4x4));

	if (flags & HAS_METALLIC)
	{
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(MaterialMetallic), nullptr, createFlags);

		metallicMaterialData = static_cast<MaterialMetallic*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, componentsInBatch
			.size() * sizeof(float4x4), mapFlags));
	}
	
	else if (flags & HAS_SPECULAR)
	{
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, componentsInBatch.size() * sizeof(MaterialSpecular), nullptr, createFlags);

		specularMaterialData = static_cast<MaterialSpecular*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, componentsInBatch
			.size() * sizeof(float4x4), mapFlags));
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindVertexArray(0);
}

void GeometryBatch::ClearBuffer()
{
	glDeleteBuffers(1, &indirectBuffer);
	
	for (int i = 0; i < DOUBLE_BUFFERS; i++)
	{
		glDeleteBuffers(1, &transforms[i]);
	}
	
	glDeleteBuffers(1, &materials);
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
		objectIndexes[newComponent] = CreateInstanceResourceMaterial(materialShared);
		newComponent->SetBatch(this);
		componentsInBatch.push_back(newComponent);
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
			if ((*it)->resourceMesh == componentToDelete->GetMesh().get())
			{
				delete (*it);
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

	fillMaterials = true;
	reserveModelSpace = true;
	dirtyBatch = true;
	componentToDelete->SetBatch(nullptr);

#else
		App->resources->FillResourceBin(componentToDelete->GetMaterial());
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
				if ((*it)->resourceMesh == compare->GetMesh().get())
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

	reserveModelSpace = true;
	fillMaterials = true;
	dirtyBatch = true;

	return componentToMove;
}


void GeometryBatch::DeleteMaterial(const ComponentMeshRenderer* componentToDelete)
{
	resourcesMaterial.erase(
			std::find(resourcesMaterial.begin(), resourcesMaterial.end(), componentToDelete->GetMaterial()));
	reserveModelSpace = true;
	//dirtyBatch = true;
}

void GeometryBatch::BindBatch(bool selected)
{
	program->Activate();

	frame = (frame + 1) % DOUBLE_BUFFERS;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointModel, transforms[frame]);
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

	float4x4* transformsAux = static_cast<float4x4*>(transformData[frame]);

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

			if (!isRoot)
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
				ResourceInfo* resourceInfo = FindResourceInfo(component->GetMesh().get());
				ResourceMesh* resource = resourceInfo->resourceMesh;
				//find position in components vector
				auto it = std::find(componentsInBatch.begin(), componentsInBatch.end(), component);

				unsigned int instanceIndex = objectIndexes[component];

				transformsAux[instanceIndex] = static_cast<ComponentTransform*>(component->GetOwner()
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
#else
			ResourceInfo* resourceInfo = FindResourceInfo(component->GetMesh().get());
			ResourceMesh* resource = resourceInfo->resourceMesh;

			//find position in components vector
			unsigned int instanceIndex = objectIndexes[component];

			transformsAux[instanceIndex] = static_cast<ComponentTransform*>(component->GetOwner()
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

void GeometryBatch::CreateInstanceResourceMesh(ResourceMesh* mesh)
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

GeometryBatch::ResourceInfo* GeometryBatch::FindResourceInfo(const ResourceMesh* mesh)
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
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &indirectBuffer);
	glDeleteBuffers(1, &verticesBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalsBuffer);
	glDeleteBuffers(1, &tangentsBuffer);
	glDeleteBuffers(DOUBLE_BUFFERS, &transforms[0]);
	glDeleteBuffers(1, &materials);

	return true;
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

		CreateInstanceResourceMesh(meshShared.get());
		reserveModelSpace = true;
	}
}

bool SortComponent(const ComponentMeshRenderer* first, const ComponentMeshRenderer* second)
{
	float firstDistance = App->GetModule<ModuleRender>()->GetObjectDistance(first->GetOwner());
	float secondDistance = App->GetModule<ModuleRender>()->GetObjectDistance(second->GetOwner());

	return (firstDistance > secondDistance);
}

void GeometryBatch::SortByDistance()
{
	std::sort(componentsInBatch.begin(), componentsInBatch.end(), SortComponent);
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
