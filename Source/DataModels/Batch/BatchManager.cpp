#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Batch/BatchFlags.h"

#include <assert.h>

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	for (GeometryBatch* batch : geometryBatchesOpaques)
	{
		delete batch;
	}
	geometryBatchesOpaques.clear();

	for (GeometryBatch* batch : geometryBatchesTransparent)
	{
		delete batch;
	}
	geometryBatchesTransparent.clear();
}

void BatchManager::AddComponent(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		int flags = 0;

		GeometryBatch* batch = CheckBatchCompatibility(newComponent, flags);

		//adapt the batch to the good one before adding the component
		std::vector<GeometryBatch*>& geometryBatches =
			newComponent->GetMaterial()->IsTransparent() ? geometryBatchesTransparent : geometryBatchesOpaques;

		if (batch)
		{
			batch->AddComponentMeshRenderer(newComponent);
		}
		else
		{
			batch = new GeometryBatch(flags);

			batch->AddComponentMeshRenderer(newComponent);
			geometryBatches.push_back(batch);
		}
	}
}

GeometryBatch* BatchManager::CheckBatchCompatibility(const ComponentMeshRenderer* newComponent, int& flags)
{
	std::shared_ptr<ResourceMesh> mesh = newComponent->GetMesh();
	std::shared_ptr<ResourceMaterial> material = newComponent->GetMaterial();

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

	if (material)
	{
		if (material->GetShaderType() == 0)
		{
			flags |= HAS_METALLIC;
		}
		else if (material->GetShaderType() == 1)
		{
			flags |= HAS_SPECULAR;
		}
	}

	//verify if it's transparent or opaque
	std::vector<GeometryBatch*>& geometry_batches = 
		newComponent->GetMaterial()->IsTransparent() ? geometryBatchesTransparent : geometryBatchesOpaques;

	for (GeometryBatch* geometryBatch : geometry_batches)
	{
		if (geometryBatch->GetFlags() == flags)
		{
			return geometryBatch;
		}
	}
	return nullptr;
}

void BatchManager::DrawOpaque(bool selected)
{
	for (GeometryBatch* geometryBatch : geometryBatchesOpaques)
	{
		if (!geometryBatch->IsEmpty())
		{
			DrawBatch(geometryBatch, selected);
		}
		else
		{
			erase_if(geometryBatchesOpaques, [](auto const& pi) { return pi->IsEmpty(); });
			delete geometryBatch;
		}
	}
}

void BatchManager::DrawTransparent(bool selected)
{
	for (GeometryBatch* geometryBatch : geometryBatchesTransparent)
	{
		if (!geometryBatch->IsEmpty())
		{
			geometryBatch->SortByDistance();
			DrawBatch(geometryBatch, selected);
		}
		else
		{
			erase_if(geometryBatchesTransparent, [](auto const& pi) { return pi->IsEmpty(); });
			delete geometryBatch;
		}
	}
}

void BatchManager::DrawBatch(GeometryBatch* batch, bool selected)
{
	if (batch->dirtyBatch)
	{
		batch->ClearBuffer();
		batch->CreateVAO();
		batch->UpdateBatchComponents();
		batch->dirtyBatch = false;

	}
	batch->BindBatch(selected);
}
